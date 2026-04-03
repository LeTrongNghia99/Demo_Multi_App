#pragma once
#include <QString>
#include <QJsonObject>
#include <QTimer>

class Message : public QObject{
    Q_OBJECT
public:
    Message(const int msgId, const QString& content, int intervalMs ,QObject *parent = nullptr );

    int msgId() const;
    QString content() const;
    int intervalMs() const;
    QTimer* timerMs() const;

    void setContent(const QString& content);
    void setIntervalMs(int intervalMs);

    // Q_INVOKABLE void startSendMessage();

private:
    int m_msgId;
    QString m_content;
    int m_intervalMs;
    QTimer* m_timerMs;
};
