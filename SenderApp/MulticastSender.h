#include "Message.h"
class Message;
#pragma once
#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QList>
#include <QPair>

class MulticastSender : public QObject
{
    Q_OBJECT
public:
    explicit MulticastSender( QObject *parent = nullptr);

    Q_INVOKABLE void addTarget(const QString &ip, int port);
    Q_INVOKABLE void removeTarget(const QString &ip, int port);
    void startSendMessage(const int id, const QString &content, int intervalMs, QTimer &timerMs);
    void stopSendMessage( Message* message);

private:
    void broadcastMessage(const int &msgId, const QString &content, int intervalMs);
    QUdpSocket m_udpSocket;
    QList<QPair<QHostAddress, quint16>> m_targets;
};
