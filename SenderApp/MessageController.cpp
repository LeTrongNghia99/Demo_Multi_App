#include "MessageController.h"
#include <algorithm>
#include <QDebug>

MessageController::MessageController(QObject *parent)
    : QAbstractListModel {parent}
{
    m_id = 0;
    m_intervalGroupMs = 0;
}

int MessageController::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_messages.size();
}

QVariant MessageController::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() < 0 || index.row() >= m_messages.size()){
        return QVariant();
    };

    Message* message = m_messages[index.row()];
    switch((Role)role){
    case IdMsg: return message->msgId();
    case ContentRole: return message->content();
    case IntervalRole: return message->intervalMs();
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> MessageController::roleNames() const
{
    QHash<int ,QByteArray > roles;
    roles[IdMsg] = "msgId";
    roles[ContentRole] = "content";
    roles[IntervalRole] = "intervalMs";
    return roles;
}


void MessageController::updateContent(int idMsg, const QString &content) {
    for (int i = 0; i < m_messages.size(); ++i) {
        if (m_messages[i]->msgId() == idMsg) {
            m_messages[i]->setContent(content);
            QModelIndex idx = index(i);
            emit dataChanged(idx, idx, {ContentRole});
            break;
        }
    }
}

void MessageController::updateInterval(int idMsg, int intervalMs) {
    for (int i = 0; i < m_messages.size(); ++i) {
        if (m_messages[i]->msgId() == idMsg) {
            m_messages[i]->setIntervalMs(intervalMs);
            QModelIndex idx = index(i);
            emit dataChanged(idx, idx, {IntervalRole});
            break;
        }
    }
}


void MessageController::addMessage() {
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    Message * message = new Message();
    message->setMsgId(m_id);
    m_messages.append(message);
    m_id++;
    endInsertRows();
}


void MessageController::deleteMessage(int idMsg) {
    for (int i = 0; i < m_messages.size(); ++i) {
        if (m_messages[i]->msgId() == idMsg) {
            beginRemoveRows(QModelIndex(), i, i);
            delete m_messages[i];
            m_messages.removeAt(i);
            endRemoveRows();
            break;
        }
    }
}

void MessageController::startSendMessage(const int &idMsg, MulticastSender *multicastSender)
{
    auto it = std::find_if(m_messages.begin(), m_messages.end(), [idMsg](Message* message) {
        return message->msgId() == idMsg;
    });
    if (it != m_messages.end()) {
        Message* foundMessage = *it;
        qDebug() << "Found:" << foundMessage->msgId();
        multicastSender->startSendMessage(foundMessage) ;
    } else {
        qDebug() << "Not found Message id" << idMsg;
    }
}

void MessageController::stopSendMessage(const int &idMsg, MulticastSender *multicastSender)
{
    auto it = std::find_if(m_messages.begin(), m_messages.end(), [idMsg](Message* message) {
        return message->msgId() == idMsg;
    });
    if (it != m_messages.end()) {
        Message* foundMessage = *it;
        qDebug() << "Found:" << foundMessage->msgId();
        multicastSender->stopSendMessage(foundMessage);
    } else {
        qDebug() << "Not found Message id" << idMsg;
    }
}

Message *MessageController::getMessage(int idMsg)
{
    auto it = std::find_if(m_messages.begin(), m_messages.end(), [idMsg](Message* message) {
        return message->msgId() == idMsg;
    });

    if (it != m_messages.end())
        return *it;
    return nullptr;
}

void MessageController::startSendGroupMessage( MulticastSender *multicastSender)
{
    multicastSender->startSendGroupMessage(m_messages,m_intervalGroupMs);
}

void MessageController::stopSendGroupMessage( MulticastSender *multicastSender)
{
    multicastSender->stopSendGroupMessage(m_messages);
}

void MessageController::sendGroupMessageFollowOder(MulticastSender *multicastSender)
{
    multicastSender->sendGroupMessageFollowOder(m_messages);
}


int MessageController::intervalGroupMs() const
{
    return m_intervalGroupMs;
}

void MessageController::setIntervalGroupMs(int newIntervalGroupMs)
{
    if (m_intervalGroupMs == newIntervalGroupMs)
        return;
    m_intervalGroupMs = newIntervalGroupMs;
    emit intervalGroupMsChanged();
}
