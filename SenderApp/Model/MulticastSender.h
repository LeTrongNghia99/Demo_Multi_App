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

    void addTarget(const QString &ip, int port);
    void removeTarget(const QString &ip, int port);
    void startSendMessage(Message* message);
    void stopSendMessage( Message* message);
    // void startSendGroupMessage(const QList<Message *> &messages, const int& intervalGroupMs);
    // void stopSendGroupMessage(const QList<Message *> &messages);
    // void sendGroupMessageFollowOder(const QList<Message *> &messages);

    void sendMessageOneTime(Message *message);
signals:
    void messageSendFinished(Message* message);

private:
    void broadcastMessage(Message* message);
    QUdpSocket m_udpSocket;
    QList<QPair<QHostAddress, quint16>> m_targets;
};
  