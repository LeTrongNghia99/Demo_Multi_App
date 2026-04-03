#include "Message.h"

Message::Message(const int msgId, const QString &content, int intervalMs , QObject *parent)
    : m_msgId(msgId), m_content(content), m_intervalMs(intervalMs) ,QObject(parent)
{
    m_timerMs = new QTimer(this);
}

int Message::msgId() const { return m_msgId; }

QString Message::content() const { return m_content; }

int Message::intervalMs() const { return m_intervalMs; }

QTimer *Message::timerMs() const { return m_timerMs; }

void Message::setContent(const QString& content) { m_content = content; }

void Message::setIntervalMs(int intervalMs) { m_intervalMs = intervalMs; }