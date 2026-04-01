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
    explicit MulticastSender(QObject *parent = nullptr);

    Q_INVOKABLE void addTarget(const QString &ip, int port);
    Q_INVOKABLE void startMs1(const QString &content, int intervalMs);
    Q_INVOKABLE void stopMs1();
    Q_INVOKABLE void startMs2(const QString &content, int intervalMs);
    Q_INVOKABLE void stopMs2();
    Q_INVOKABLE void sendMs3(const QString &content);

private:
    void broadcastMessage(const QString &msgId, const QString &content, int intervalMs);
    QByteArray createJsonMessage(const QString &msgId, const QString &content, int intervalMs) const;

    QUdpSocket m_udpSocket;
    QTimer m_timerMs1;
    QTimer m_timerMs2;
    QList<QPair<QHostAddress, quint16>> m_targets;
    QString m_ms1Content;
    int m_ms1Interval;
    QString m_ms2Content;
    int m_ms2Interval;
};
