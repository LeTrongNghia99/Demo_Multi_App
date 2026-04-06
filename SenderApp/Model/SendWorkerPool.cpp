#include "SendWorkerPool.h"
#include <QtConcurrent>
#include <QDebug>
#include <QException>

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

void SendWorkerPool::sendMessage( Message* msg, MulticastSender* sender)
{

    // Connect MulticastSender signal → pool signal
    connect(sender, &MulticastSender::messageSendFinished,
            this, [this, msg](Message* finishedMsg) {
                // Callback khi MulticastSender emit messageSendFinished
                if (finishedMsg->msgId() == msg->msgId()) {
                    qDebug() << "[SendWorkerPool] Detected send finished:" << msg->msgId();

                    // WHY: Emit từ main thread (automatic via Qt marshalling)
                    // Signal từ sender→ slot chạy main thread
                    emit sendFinished(msg->msgId());

                    // Cleanup
                    {
                        QMutexLocker locker(&m_mutex);
                        m_activeSends.remove(msg->msgId());
                    }
                }
            },
            Qt::UniqueConnection);  // WHY: UniqueConnection?
                                    // Tránh duplicate connections nếu sendMessage call nhiều lần

    QFuture<void> future = QtConcurrent::run(m_threadPool,[this, msg, sender]() {
        try{
            qDebug() << "[SendWorker] Start sending message" << msg->msgId()
                  << "on thread" << QThread::currentThreadId();

            // Gửi message trên background thread
            // NIU: startSendMessage() sẽ block cho đến khi send xong
            // Vì chạy trên worker thread, nên UI không bị block
            sender->startSendMessage(msg);

            qDebug() << "[SendWorker] Finished sending message" << msg->msgId();

        } catch (const std::exception& e) {
            qWarning() << "[SendWorker] Error sending message" << msg->msgId()
                    << ":" << e.what();
            emit sendError(msg->msgId(), QString::fromStdString(e.what()));
        }
    });

    // WHY: Lock mutex khi modify m_activeSends?
    // Vì main thread có thể call cancelSend() lúc worker thread read m_activeSends
    // Mutex đảm bảo data consistency
    {
        QMutexLocker locker(&m_mutex);  // RAII: auto unlock khi scope exit
        m_activeSends[msg->msgId()] = future;
    }

    qDebug() << "[SendWorkerPool] Queued message" << msg->msgId()
             << "- Active sends:" << activeCount();
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
    sender->sendMessageOneTime(messages[0]);

    for (int i = 1; i < messages.size(); ++i) {
        QTimer::singleShot(i*1000, this, [this, messages, sender, i]() {  // ← VALUE không &
            Message* msg = messages[i];  // ✓ Safe - messages là local copy
            QFuture<void> future = QtConcurrent::run(m_threadPool,[this, msg, sender]() {
                try{
                    qDebug() << "[SendWorker] Start sending message" << msg->msgId()
                    << "on thread" << QThread::currentThreadId();

                    sender->sendMessageOneTime(msg);

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



// void SendWorkerPool::cancelSend(int msgId)
// {

//     QMutexLocker locker(&m_mutex);

//     // WHY: Check nếu msgId exists trước cancel?
//     // Vì QMap::cancel() crash nếu key không exist
//     if (m_activeSends.contains(msgId)) {
//         // WHY: QFuture::cancel() hủy task nếu chưa start
//         // Nếu đã start, cancel sẽ fail (return false) nhưng không crash
//         m_activeSends[msgId].cancel();

//         // WHY: remove từ map?
//         // Cleanup - tránh map bị bloat
//         m_activeSends.remove(msgId);

//         qDebug() << "[SendWorkerPool] Cancel message" << msgId;
//     }
// }

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