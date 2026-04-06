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
    Q_INVOKABLE void startSendMessage(Message* message);
    Q_INVOKABLE void stopSendMessage( Message* message);
    void startSendGroupMessage(const QList<Message *> &messages, const int& intervalGroupMs);
    void stopSendGroupMessage(const QList<Message *> &messages);
    void sendGroupMessageFollowOder(const QList<Message *> &messages);

private:
    void broadcastMessage(Message* message);
    QUdpSocket m_udpSocket;
    QList<QPair<QHostAddress, quint16>> m_targets;
};
  