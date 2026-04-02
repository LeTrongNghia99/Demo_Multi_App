#pragma once
#include <QString>
#include <QJsonObject>

class Message {
public:
    Message(const QString& msgId, const QString& content, int intervalMs)
        : m_msgId(msgId), m_content(content), m_intervalMs(intervalMs) {}

    QString msgId() const { return m_msgId; }
    QString content() const { return m_content; }
    int intervalMs() const { return m_intervalMs; }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["msg_id"] = m_msgId;
        obj["content"] = m_content;
        obj["interval_ms"] = m_intervalMs;
        return obj;
    }

private:
    QString m_msgId;
    QString m_content;
    int m_intervalMs;
};
