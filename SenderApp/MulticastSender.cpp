
#include "MulticastSender.h"
#include "Message.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>


MulticastSender::MulticastSender(QObject *parent)
    : QObject(parent)
{
    qDebug() << "[Sender] MulticastSender constructed";
    connect(&m_timerMs1, &QTimer::timeout, this, [this]() {
        qDebug() << "[Sender] Timer MS1 timeout, sending message";
        for (const Message* msg : m_messages) {
            if (msg->msgId() == "ms1") {
                broadcastMessage(msg->msgId(), msg->content(), msg->intervalMs());
            }
        }
    });
    connect(&m_timerMs2, &QTimer::timeout, this, [this]() {
        qDebug() << "[Sender] Timer MS2 timeout, sending message";
        for (const Message* msg : m_messages) {
            if (msg->msgId() == "ms2") {
                broadcastMessage(msg->msgId(), msg->content(), msg->intervalMs());
            }
        }
    });
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


void MulticastSender::startMs1(const QString &content, int intervalMs)
{
    qDebug() << "[Sender] startMs1 called, content:" << content << ", intervalMs:" << intervalMs;
    // Tạo object Message động cho ms1
    Message* msg = new Message("ms1", content, intervalMs);
    m_messages.append(msg);
    m_timerMs1.start(intervalMs);
    // Gửi ngay message vừa tạo
    broadcastMessage(msg->msgId(), msg->content(), msg->intervalMs());
}


void MulticastSender::stopMs1()
{
    qDebug() << "[Sender] stopMs1 called";
    m_timerMs1.stop();
    // Xóa các Message có msgId == "ms1"
    for (int i = m_messages.size() - 1; i >= 0; --i) {
        if (m_messages[i]->msgId() == "ms1") {
            delete m_messages[i];
            m_messages.removeAt(i);
        }
    }
}


void MulticastSender::startMs2(const QString &content, int intervalMs)
{
    qDebug() << "[Sender] startMs2 called, content:" << content << ", intervalMs:" << intervalMs;
    // Tạo object Message động cho ms2
    Message* msg = new Message("ms2", content, intervalMs);
    m_messages.append(msg);
    m_timerMs2.start(intervalMs);
    // Gửi ngay message vừa tạo
    broadcastMessage(msg->msgId(), msg->content(), msg->intervalMs());
}


void MulticastSender::stopMs2()
{
    qDebug() << "[Sender] stopMs2 called";
    m_timerMs2.stop();
    // Xóa các Message có msgId == "ms2"
    for (int i = m_messages.size() - 1; i >= 0; --i) {
        if (m_messages[i]->msgId() == "ms2") {
            delete m_messages[i];
            m_messages.removeAt(i);
        }
    }
}


void MulticastSender::sendMs3(const QString &content)
{
    qDebug() << "[Sender] sendMs3 called, content:" << content;
    // Tạo object Message động cho ms3 (one-time)
    Message* msg = new Message("ms3", content, 0);
    m_messages.append(msg);
    broadcastMessage(msg->msgId(), msg->content(), msg->intervalMs());
    // Xóa ngay sau khi gửi
    for (int i = m_messages.size() - 1; i >= 0; --i) {
        if (m_messages[i]->msgId() == "ms3") {
            delete m_messages[i];
            m_messages.removeAt(i);
        }
    }
}


void MulticastSender::broadcastMessage(const QString &msgId, const QString &content, int intervalMs)
{
    // Serialize tất cả message thành JSON và gửi
    for (const Message* msg : m_messages) {
        QByteArray datagram = QJsonDocument(msg->toJson()).toJson(QJsonDocument::Compact);
        qDebug() << "[Sender] broadcastMessage called, msgId:" << msg->msgId() << ", content:" << msg->content() << ", intervalMs:" << msg->intervalMs() << ", targets:" << m_targets.size();
        for (const auto &target : m_targets) {
            qDebug() << "[Sender] Sending datagram to" << target.first.toString() << ":" << target.second << ", data:" << datagram;
            m_udpSocket.writeDatagram(datagram, target.first, target.second);
        }
    }
}

QByteArray MulticastSender::createJsonMessage(const QString &msgId, const QString &content, int intervalMs) const
{
    QJsonObject obj;
    obj["msg_id"] = msgId;
    obj["content"] = content;
    obj["interval_ms"] = intervalMs;
    obj["timestamp"] = static_cast<qint64>(QDateTime::currentSecsSinceEpoch());
    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Compact);
}
