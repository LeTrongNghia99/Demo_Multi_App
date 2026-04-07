#include "MessageController.h"
#include "../Model/MessageManager.h"
#include "../Model/MulticastSender.h"
#include <QDebug>

MessageController::MessageController(QObject *parent)
    : QObject(parent), m_intervalGroupMs(0)
{
    m_sendWorkerPool = new SendWorkerPool(4, this);
}

void MessageController::setMessageManager(MessageManager* messageManager)
{
    m_messageManager = messageManager;
}

void MessageController::setMulticastSender(MulticastSender* multicastSender)
{
    m_multicastSender = multicastSender;
}

void MessageController::addTarget(const QString &ip, int port)
{
    if (!m_multicastSender) {
        qWarning() << "[MessageController] MulticastSender not set!";
        return;
    }
    m_multicastSender->addTarget(ip, port);
    qDebug() << "[MessageController] Added target:" << ip << port;
}

void MessageController::removeTarget(const QString &ip, int port)
{
    if (!m_multicastSender) {
        qWarning() << "[MessageController] MulticastSender not set!";
        return;
    }
    m_multicastSender->removeTarget(ip, port);
    qDebug() << "[MessageController] Removed target:" << ip << port;
}

void MessageController::startSendMessage(Message* message)
{
    if (!m_messageManager || !m_multicastSender || !m_sendWorkerPool) {
        qWarning() << "[MessageController] MessageManager or MulticastSender not set!";
        return;
    }
    
    if (message) {
        qDebug() << "[MessageController] Starting send for message" << message->msgId();
        m_sendWorkerPool->sendMessage(message , m_multicastSender);
    } else {
        qWarning() << "[MessageController] Message not found:" << message->msgId();
    }
}

void MessageController::stopSendMessage(Message* message)
{
    if (!m_messageManager || !m_multicastSender || !m_sendWorkerPool) {
        qWarning() << "[MessageController] MessageManager or MulticastSender not set!";
        return;
    }
    
    if (message) {
        qDebug() << "[MessageController] Stopping send for message" << message->msgId();
        m_sendWorkerPool->cancelSend(message);
    } else {
        qWarning() << "[MessageController] Message not found:" << message->msgId();
    }
}

void MessageController::startSendGroupMessage()
{
    if (!m_messageManager || !m_multicastSender || !m_sendWorkerPool) {
        qWarning() << "[MessageController] MessageManager or MulticastSender not set!";
        return;
    }
    
    QList<Message*> messages = m_messageManager->getAllMessages();
    qDebug() << "[MessageController] Starting group send with" << messages.size() << "messages";
    // m_multicastSender->startSendGroupMessage(messages, m_intervalGroupMs);
    for (int i = 0; i < messages.size(); ++i){
        messages[i]->setIsStopped(false);
    }
    m_sendWorkerPool->sendGroupMessages(messages,m_multicastSender, m_intervalGroupMs);
}

void MessageController::stopSendGroupMessage()
{
    if (!m_messageManager || !m_multicastSender) {
        qWarning() << "[MessageController] MessageManager or MulticastSender not set!";
        return;
    }
    
    QList<Message*> messages = m_messageManager->getAllMessages();
    qDebug() << "[MessageController] Stopping group send";
    for (int i = 0; i < messages.size(); ++i){
        messages[i]->setIsStopped(true);
    }
}

void MessageController::sendGroupMessageFollowOder()
{
    if (!m_messageManager || !m_multicastSender) {
        qWarning() << "[MessageController] MessageManager or MulticastSender not set!";
        return;
    }
    
    QList<Message*> messages = m_messageManager->getAllMessages();
    qDebug() << "[MessageController] Sending group messages in order";
    m_sendWorkerPool->sendGroupMessageFollowOder(messages, m_multicastSender);
    // m_multicastSender->sendGroupMessageFollowOder(messages);
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
