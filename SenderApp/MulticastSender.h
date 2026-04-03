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
    // Q_INVOKABLE void startMs1(const QString &content, int intervalMs);
    // Q_INVOKABLE void stopMs1();
    // Q_INVOKABLE void startMs2(const QString &content, int intervalMs);
    // Q_INVOKABLE void stopMs2();
    // Q_INVOKABLE void sendMs3(const QString &content);

    void startSendMessage(const int id, const QString &content, int intervalMs, QTimer &timerMs);
    void stopSendMessage( Message* message);

private:
    // static int msgId ;
    void broadcastMessage(const int &msgId, const QString &content, int intervalMs);
    // QByteArray createJsonMessage(const int &msgId, const QString &content, int intervalMs) const;

    QUdpSocket m_udpSocket;
    QTimer m_timerMs1;
    QTimer m_timerMs2;
    QList<QPair<QHostAddress, quint16>> m_targets;
};
