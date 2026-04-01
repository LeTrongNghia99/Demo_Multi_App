#pragma once

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class MulticastReceiver : public QObject
{
    Q_OBJECT
public:
    explicit MulticastReceiver(QObject *parent = nullptr);

    Q_INVOKABLE bool joinGroup(const QString &ip, int port);

signals:
    void messageReceived(const QString &msgId, const QString &content, const QString &timeStr);

private slots:
    void readPendingDatagrams();

private:
    QUdpSocket m_udpSocket;
};
