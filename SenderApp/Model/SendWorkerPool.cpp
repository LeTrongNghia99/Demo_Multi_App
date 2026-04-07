#include "SendWorkerPool.h"
#include <QtConcurrent>
#include <QDebug>
#include <QException>
#include <QTimer>

SendWorkerPool::SendWorkerPool(int maxThreads, QObject* parent)
    : QObject(parent), m_maxThreads(maxThreads)
{
    // Create thread pool với max threads
    m_threadPool = new QThreadPool(this);
    m_threadPool->setMaxThreadCount(maxThreads);

    qDebug() << "[SendWorkerPool] Created with max threads:" << maxThreads;
}

SendWorkerPool::~SendWorkerPool()
{
    // WHY: Cleanup - wait cho tất cả worker threads finish
    // Nếu không wait, threads có thể bị kill khi destructor chạy → data corruption
    cancelAll();
    m_threadPool->waitForDone();

    qDebug() << "[SendWorkerPool] Destroyed";
}

void SendWorkerPool::sendMessage(Message* msg, MulticastSender* sender)
{
    if (!msg || !sender) {
        qWarning() << "[SendWorkerPool] Invalid message or sender";
        return;
    }

    int msgId = msg->msgId();
    qDebug() << "[SendWorkerPool] Starting send for message" << msgId
             << "with interval" << msg->intervalMs() << "ms";

    QTimer* timer = msg->timerMs();
    
    // ✅ Disconnect cũ trước (nếu sendMessage được gọi lại cho message này)
    disconnect(timer, nullptr, this, nullptr);
    
    // Lambda để dispatch send task
    // ⚠️ Không dùng UniqueConnection vì lambda object khác nhau mỗi lần
    auto sendTask = [this, msg, sender, msgId]() {
        // Check nếu message bị stopped
        if (msg->isStopped()) {
            qDebug() << "[SendWorkerPool] Message" << msgId << "stopped, skipping send";
            return;
        }

        // Dispatch send task vào thread pool
        QFuture<void> future = QtConcurrent::run(m_threadPool, [this, msg, sender, msgId]() {
            try {
                qDebug() << "[SendWorker] Sending message" << msgId
                         << "on thread" << QThread::currentThreadId();
                
                // ✅ Chỉ gửi broadcast (không loop + sleep!)
                sender->broadcastMessage(msg);
                
                qDebug() << "[SendWorker] Send completed for message" << msgId;
            } catch (const std::exception& e) {
                qWarning() << "[SendWorker] Error sending message" << msgId
                           << ":" << e.what();
                emit sendError(msgId, QString::fromStdString(e.what()));
            }
        });

        {
            QMutexLocker locker(&m_mutex);
            m_activeSends[msgId] = future;
        }

        qDebug() << "[SendWorkerPool] Dispatched send for message" << msgId
                 << "- Active sends:" << activeCount();
    };

    // Connect timer timeout → gửi message
    // ❌ Không dùng UniqueConnection (gây problem với lambda)
    connect(timer, &QTimer::timeout, this, sendTask);

    // Gửi lần đầu ngay lập tức
    sendTask();

    // Set interval từ message và start timer
    timer->setInterval(msg->intervalMs());
    timer->start();

    qDebug() << "[SendWorkerPool] Timer started for message" << msgId
             << "with interval" << msg->intervalMs() << "ms";
}


void SendWorkerPool::sendGroupMessages( const QList<Message*>& msgs,
                                       MulticastSender* sender, const int& intervalGroupMs)
{
    // WHY: Gửi tất cả messages song song?
    // Nếu 10 messages × 1000ms = 10000ms nếu sequential
    // Nếu song song trên 4 threads = ~2500ms (4x speedup)


    for (int i = 0; i < msgs.size(); ++i) {
        Message* msg = msgs[i];
        msg->setIntervalMs(intervalGroupMs);
        sendMessage( msgs[i], sender);
    }

    qDebug() << "[SendWorkerPool] Queued" << msgs.size()
             << "group messages - Active sends:" << activeCount();
}

void SendWorkerPool::sendGroupMessageFollowOder(const QList<Message *> &messages, MulticastSender* sender)
{
    if (messages.isEmpty()) return;
    sender->startSendMessage(messages[0]);

    for (int i = 1; i < messages.size(); ++i) {
        QTimer::singleShot(i*1000, this, [this, messages, sender, i]() {  // ← VALUE không &
            Message* msg = messages[i];  // ✓ Safe - messages là local copy
            QFuture<void> future = QtConcurrent::run(m_threadPool,[this, msg, sender]() {
                try{
                    qDebug() << "[SendWorker] Start sending message" << msg->msgId()
                    << "on thread" << QThread::currentThreadId();

                    sender->startSendMessage(msg);

                } catch (const std::exception& e) {
                    qWarning() << "[SendWorker] Error sending message" << msg->msgId()
                    << ":" << e.what();
                    emit sendError(msg->msgId(), QString::fromStdString(e.what()));
                }
            });

            {
                QMutexLocker locker(&m_mutex);
                m_activeSends[msg->msgId()] = future;
            }
        });
    }
}



void SendWorkerPool::cancelSend(Message* message)
{
    int msgId = message->msgId();
    QMutexLocker locker(&m_mutex);

    // WHY: Check nếu msgId exists trước cancel?
    // Vì QMap::cancel() crash nếu key không exist
    if (m_activeSends.contains(msgId)) {

        message->timerMs()->stop();

        // WHY: QFuture::cancel() hủy task nếu chưa start
        // Nếu đã start, cancel sẽ fail (return false) nhưng không crash
        m_activeSends[msgId].cancel();

        // WHY: remove từ map?
        // Cleanup - tránh map bị bloat
        m_activeSends.remove(msgId);



        qDebug() << "[SendWorkerPool] Cancel message" << msgId;
    }
}

void SendWorkerPool::cancelAll()
{
    QMutexLocker locker(&m_mutex);

    // WHY: Iterate & cancel all?
    // Cleanup tất cả ongoing operations khi shutdown
    for (auto it = m_activeSends.begin(); it != m_activeSends.end(); ++it) {
        it.value().cancel();
    }

    m_activeSends.clear();

    qDebug() << "[SendWorkerPool] Cancelled all sends";
}

bool SendWorkerPool::isRunning(int msgId) const
{
    QMutexLocker locker(&m_mutex);

    // WHY: isRunning() vừa check exist vừa check running?
    // true nếu: msgId trong m_activeSends AND chưa finished
    return m_activeSends.contains(msgId) &&
           m_activeSends[msgId].isRunning();
}

int SendWorkerPool::activeCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_activeSends.size();
}

void SendWorkerPool::setMaxThreads(int maxThreads)
{
    // WHY: Có thể change max threads after creation?
    // Dynamic adjustment - nếu app detect high CPU, có thể reduce threads
    m_maxThreads = maxThreads;
    m_threadPool->setMaxThreadCount(maxThreads);

    qDebug() << "[SendWorkerPool] Max threads updated to:" << maxThreads;
}