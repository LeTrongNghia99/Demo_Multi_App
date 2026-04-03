
#include "MulticastSender.h"
#include "Message.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>


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

void MulticastSender::startSendMessage(const int id, const QString &content, int intervalMs , QTimer &timerMs)
{
    qDebug() << "[Sender] Message " << id << " called, content:" << content << ", intervalMs:" << intervalMs;
    broadcastMessage(id, content, intervalMs);
    timerMs.disconnect();
    connect(&timerMs, &QTimer::timeout, this, [this, id, content, intervalMs]() {
        qDebug() << "[Sender] Timer" << id << " timeout, sending message";
        broadcastMessage(id, content, intervalMs);
    });
    timerMs.start(intervalMs);
}

void MulticastSender::stopSendMessage( Message* message)
{
    message->timerMs()->stop();
}

void MulticastSender::broadcastMessage(const int &msgId, const QString &content, int intervalMs)
{
    // 1. Đóng gói tham số vào JSON
    QJsonObject jsonObj;
    jsonObj["msgId"] = msgId;
    jsonObj["content"] = content;
    jsonObj["interval"] = intervalMs;
    jsonObj["timestamp"] = static_cast<qint64>(QDateTime::currentSecsSinceEpoch());

    // 2. Chuyển đổi sang QByteArray (Compact để tối ưu gói tin UDP)
    QByteArray datagram = QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);

    // 3. Kiểm tra danh sách đích
    if (m_targets.isEmpty()) {
        qWarning() << "[Sender] No targets defined to broadcast.";
        return;
    }

    // 4. Gửi đến tất cả các đích (targets)
    qDebug() << "[Sender] Broadcasting Message ID:" << msgId << "to" << m_targets.size() << "targets";

    for (const auto &target : std::as_const(m_targets)) {
        // target.first thường là QHostAddress, target.second là port (quint16)
        qint64 bytesWritten = m_udpSocket.writeDatagram(datagram, target.first, target.second);

        if (bytesWritten == -1) {
            qCritical() << "[Sender] Failed to send to" << target.first.toString() << ":" << target.second;
        }
    }
}
