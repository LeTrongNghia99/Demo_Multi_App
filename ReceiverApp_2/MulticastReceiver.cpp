
#include "MulticastReceiver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>


MulticastReceiver::MulticastReceiver(QObject *parent)
    : QObject(parent)
{
    qDebug() << "[ReceiverApp_2] MulticastReceiver constructed";
    connect(&m_udpSocket, &QUdpSocket::readyRead, this, &MulticastReceiver::readPendingDatagrams);
}


bool MulticastReceiver::joinGroup(const QString &ip, int port)
{
    QString groupIp = ip.trimmed().isEmpty() ? "239.255.0.1" : ip;
    int groupPort = (port <= 0) ? 6000 : port;
    qDebug() << "[ReceiverApp_2] joinGroup called with ip:" << groupIp << ", port:" << groupPort;
    if (m_udpSocket.state() != QAbstractSocket::UnconnectedState)
        m_udpSocket.close();
    bool ok = m_udpSocket.bind(QHostAddress::AnyIPv4, groupPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    if (!ok) {
        qDebug() << "[ReceiverApp_2] bind failed for port" << groupPort;
        return false;
    }
    ok = m_udpSocket.joinMulticastGroup(QHostAddress(groupIp));
    if (!ok) {
        qDebug() << "[ReceiverApp_2] joinMulticastGroup failed for ip" << groupIp;
        return false;
    }
    m_udpSocket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
    qDebug() << "[ReceiverApp_2] joinGroup success for ip:" << groupIp << ", port:" << groupPort;
    return true;
}


void MulticastReceiver::readPendingDatagrams()
{
    qDebug() << "[ReceiverApp_2] readPendingDatagrams called";
    while (m_udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(int(m_udpSocket.pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;
        m_udpSocket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        qDebug() << "[ReceiverApp_2] Datagram received from" << sender.toString() << ":" << senderPort << ", data:" << datagram;

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(datagram, &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            qDebug() << "[ReceiverApp_2] JSON parse error or not object:" << parseError.errorString();
            continue;
        }
        QJsonObject obj = doc.object();
        QString msgId = obj.value("msgId").toString();
        QString content = obj.value("content").toString();
        qint64 timestamp = obj.value("timestamp").toVariant().toLongLong();
        QString timeStr = QDateTime::fromSecsSinceEpoch(timestamp).toString("yyyy-MM-dd HH:mm:ss");
        qDebug() << "[ReceiverApp_2] Parsed message: msgId=" << msgId << ", content=" << content << ", timeStr=" << timeStr;
        emit messageReceived(msgId, content, timeStr);
    }
}
