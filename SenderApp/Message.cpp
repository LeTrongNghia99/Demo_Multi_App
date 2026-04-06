#include "Message.h"

Message::Message(QObject *parent)
    : QObject(parent)
{
    m_timerMs = new QTimer(this);
    m_customFieldModel = new CustomFieldModel(this);
    m_intervalMs = 1000;
}

int Message::msgId() const { return m_msgId; }

QString Message::content() const { return m_content; }

int Message::intervalMs() const { return m_intervalMs; }

QTimer *Message::timerMs() const { return m_timerMs; }

void Message::setContent(const QString& content) { m_content = content; }

void Message::setIntervalMs(int intervalMs) { m_intervalMs = intervalMs; }

CustomFieldModel *Message::customFieldModel() const
{
    return m_customFieldModel;
}

void Message::setCustomFieldModel(CustomFieldModel *newCustomFieldModel)
{
    if (m_customFieldModel == newCustomFieldModel)
        return;
    m_customFieldModel = newCustomFieldModel;
    emit customFieldModelChanged();
}

void Message::setMsgId(int newMsgId)
{
    if (m_msgId == newMsgId)
        return;
    m_msgId = newMsgId;
    emit msgIdChanged();
}
