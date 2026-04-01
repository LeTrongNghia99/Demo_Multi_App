
#include "MulticastSender.h"
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
        broadcastMessage("ms1", m_ms1Content, m_ms1Interval);
    });
    connect(&m_timerMs2, &QTimer::timeout, this, [this]() {
        qDebug() << "[Sender] Timer MS2 timeout, sending message";
        broadcastMessage("ms2", m_ms2Content, m_ms2Interval);
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
    m_ms1Content = content;
    m_ms1Interval = intervalMs;
    m_timerMs1.start(intervalMs);
    broadcastMessage("ms1", content, intervalMs); // Send immediately
}


void MulticastSender::stopMs1()
{
    qDebug() << "[Sender] stopMs1 called";
    m_timerMs1.stop();
}


void MulticastSender::startMs2(const QString &content, int intervalMs)
{
    qDebug() << "[Sender] startMs2 called, content:" << content << ", intervalMs:" << intervalMs;
    m_ms2Content = content;
    m_ms2Interval = intervalMs;
    m_timerMs2.start(intervalMs);
    broadcastMessage("ms2", content, intervalMs); // Send immediately
}


void MulticastSender::stopMs2()
{
    qDebug() << "[Sender] stopMs2 called";
    m_timerMs2.stop();
}


void MulticastSender::sendMs3(const QString &content)
{
    qDebug() << "[Sender] sendMs3 called, content:" << content;
    broadcastMessage("ms3", content, 0);
}


void MulticastSender::broadcastMessage(const QString &msgId, const QString &content, int intervalMs)
{
    QByteArray datagram = createJsonMessage(msgId, content, intervalMs);
    qDebug() << "[Sender] broadcastMessage called, msgId:" << msgId << ", content:" << content << ", intervalMs:" << intervalMs << ", targets:" << m_targets.size();
    for (const auto &target : m_targets) {
        qDebug() << "[Sender] Sending datagram to" << target.first.toString() << ":" << target.second << ", data:" << datagram;
        m_udpSocket.writeDatagram(datagram, target.first, target.second);
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
