
#include "MulticastSender.h"
#include "Message.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <thread>

MulticastSender::MulticastSender( QObject *parent)
    :  QObject(parent)
{
    qDebug() << "[Sender] MulticastSender constructed";
}


void MulticastSender::addTarget(const QString &ip, int port)
{
    QString groupIp = ip.trimmed().isEmpty() ? "239.255.0.1" : ip;
    int groupPort = (port <= 0) ? 6000 : port;
    qDebug() << "[Sender] addTarget called with ip:" << groupIp << ", port:" << groupPort;
    QHostAddress addr(groupIp);
    for (auto &pair : m_targets) {
        if (pair.first == addr && pair.second == groupPort) {
            qDebug() << "[Sender] Target already exists:" << groupIp << groupPort;
            return; // Already exists
        }
    }
    m_targets.append(qMakePair(addr, static_cast<quint16>(groupPort)));
    qDebug() << "[Sender] Target added:" << groupIp << groupPort;
}

void MulticastSender::removeTarget(const QString &ip, int port)
{
    QString groupIp = ip.trimmed().isEmpty() ? "239.255.0.1" : ip;
    int groupPort = (port <= 0) ? 6000 : port;
    QHostAddress addr(groupIp);
    bool removed = false;
    for (int i = 0; i < m_targets.size(); ++i) {
        if (m_targets[i].first == addr && m_targets[i].second == groupPort) {
            m_targets.removeAt(i);
            qDebug() << "[Sender] Target removed:" << groupIp << groupPort;
            removed = true;
            break;
        }
    }
    if (!removed) {
        qDebug() << "[Sender] Target not found to remove:" << groupIp << groupPort;
    }
}

void MulticastSender::startSendMessage(Message *message)
{
    qDebug() << "[Sender] Message " << message->msgId() << " called, content:" << message->content() << ", intervalMs:" << message->intervalMs();
    broadcastMessage(message);
    emit messageSendFinished(message);

    while (!message->isStopped()) {
        std::unique_lock<std::mutex> lock(*message->stopMutex());

        // Sleep nhưng có thể bị interrupt nếu user stop
        message->stopCV()->wait_for(
            lock,
            std::chrono::milliseconds(message->intervalMs()),
            [message]() { return message->isStopped(); }
        );

        if (message->isStopped()) {
            qDebug() << "[Sender] Message" << message->msgId() << "stopped";
            break;
        }

        broadcastMessage(message);
        emit messageSendFinished(message);
    }
}

void MulticastSender::stopSendMessage( Message* message)
{
    if (message) {
        message->setIsStopped(true);  // ← Auto notify CV
    }
}

// void MulticastSender::startSendGroupMessage(const QList<Message *> &messages, const int& intervalGroupMs)
// {
//     for (int i = 0; i < messages.size(); ++i) {

//         Message* msg = messages[i];
//         msg->setIntervalMs(intervalGroupMs);

//         // Gửi lần đầu theo thứ tự, mỗi message delay i*100 ms so với message trước đó để đảm bảo thứ tự khi gửi
//         QTimer::singleShot(i*100 , this, [this, msg]() {
//             startSendMessage(msg); // startSendMessage sẽ gửi lần đầu và start timer lặp lại
//         });
//     }
// }

// void MulticastSender::stopSendGroupMessage(const QList<Message *> &messages)
// {
//     for (int i = 0; i < messages.size(); ++i) {
//         stopSendMessage(messages[i]);
//     }
// }

// void MulticastSender::sendGroupMessageFollowOder(const QList<Message *> &messages)
// {
//     if (messages.isEmpty()) return;
//     broadcastMessage(messages[0]);

//     for (int i = 1; i < messages.size(); ++i) {
//         QTimer::singleShot(i*1000, this, [this ,&messages, i]() {
//             broadcastMessage(messages[i]);
//         });
//     }
// }

void MulticastSender::sendMessageOneTime(Message* message)
{
    if (message) {
        broadcastMessage(message);
    }
}


void MulticastSender::broadcastMessage(Message* message)
{
    // 1. Đóng gói tham số vào JSON
    QJsonObject jsonObj;
    jsonObj["msgId"] = message->msgId();
    jsonObj["content"] = message->content();
    jsonObj["interval"] = message->intervalMs();
    jsonObj["timestamp"] = static_cast<qint64>(QDateTime::currentSecsSinceEpoch());

    QList<CustomField*> customFieldList = message->customFieldModel()->fields();
    for(int i = 0; i < customFieldList.size(); i++ ){
        qDebug() << "[Sender App] Key " << customFieldList[i]->key() << " value " << customFieldList[i]->value() ;
        jsonObj[customFieldList[i]->key()] = customFieldList[i]->value();
    }

    // 2. Chuyển đổi sang QByteArray (Compact để tối ưu gói tin UDP)
    QByteArray datagram = QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);

    // 3. Kiểm tra danh sách đích
    if (m_targets.isEmpty()) {
        qWarning() << "[Sender] No targets defined to broadcast.";
        return;
    }

    // 4. Gửi đến tất cả các đích (targets)
    qDebug() << "[Sender] Broadcasting Message ID:" << message->msgId() << "to" << m_targets.size() << "targets";

    for (const auto &target : std::as_const(m_targets)) {
        // target.first thường là QHostAddress, target.second là port (quint16)
        qint64 bytesWritten = m_udpSocket.writeDatagram(datagram, target.first, target.second);

        if (bytesWritten == -1) {
            qCritical() << "[Sender] Failed to send to" << target.first.toString() << ":" << target.second;
        }
    }
    
    // WHY: Sleep 2000ms?
    // Giả vờ broadcast là task nặng, cần 2 giây để simulate real work
    qDebug() << "[Sender] Simulating heavy task... sleeping 2000ms";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}
