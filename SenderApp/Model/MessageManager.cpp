#include "MessageManager.h"
#include <algorithm>
#include <QDebug>

MessageManager::MessageManager(QObject *parent)
    : QAbstractListModel(parent), m_nextId(0)
{
}

MessageManager::~MessageManager()
{
    qDeleteAll(m_messages);
}

int MessageManager::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_messages.size();
}

QVariant MessageManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_messages.size()) {
        return QVariant();
    }

    Message* message = m_messages[index.row()];
    switch ((Role)role) {
    case IdMsg:
        return message->msgId();
    case ContentRole:
        return message->content();
    case IntervalRole:
        return message->intervalMs();
    default:
        break;
    }
    return QVariant();
}

QHash<int, QByteArray> MessageManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdMsg] = "msgId";
    roles[ContentRole] = "content";
    roles[IntervalRole] = "intervalMs";
    return roles;
}

void MessageManager::addMessage()
{
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    Message* message = new Message(this);
    message->setMsgId(m_nextId++);
    m_messages.append(message);
    endInsertRows();
}

void MessageManager::deleteMessage(int idMsg)
{
    for (int i = 0; i < m_messages.size(); ++i) {
        if (m_messages[i]->msgId() == idMsg) {
            beginRemoveRows(QModelIndex(), i, i);
            delete m_messages[i];
            m_messages.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}

Message* MessageManager::getMessage(int idMsg) const
{
    auto it = std::find_if(m_messages.begin(), m_messages.end(),
                           [idMsg](Message* msg) { return msg->msgId() == idMsg; });
    return (it != m_messages.end()) ? *it : nullptr;
}

int MessageManager::messageCount() const
{
    return m_messages.size();
}

QList<Message*> MessageManager::getAllMessages() const
{
    return m_messages;
}

void MessageManager::updateContent(int idMsg, const QString &content)
{
    for (int i = 0; i < m_messages.size(); ++i) {
        if (m_messages[i]->msgId() == idMsg) {
            m_messages[i]->setContent(content);
            QModelIndex idx = index(i);
            emit dataChanged(idx, idx, {ContentRole});
            return;
        }
    }
}

void MessageManager::updateInterval(int idMsg, int intervalMs)
{
    for (int i = 0; i < m_messages.size(); ++i) {
        if (m_messages[i]->msgId() == idMsg) {
            m_messages[i]->setIntervalMs(intervalMs);
            QModelIndex idx = index(i);
            emit dataChanged(idx, idx, {IntervalRole});
            return;
        }
    }
}