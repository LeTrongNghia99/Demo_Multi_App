#ifndef MESSAGECONTROLLER_H
#define MESSAGECONTROLLER_H

#include "../Model/MessageManager.h"
#include "../Model/MulticastSender.h"
#include "../Model/SendWorkerPool.h"
#include <QObject>

class MessageController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int intervalGroupMs READ intervalGroupMs WRITE setIntervalGroupMs NOTIFY intervalGroupMsChanged FINAL)

public:
    explicit MessageController(QObject *parent = nullptr);
    
    // Set dependencies (Model và Sender)
    void setMessageManager(MessageManager* messageManager);
    void setMulticastSender(MulticastSender* multicastSender);

    // Network control
    Q_INVOKABLE void addTarget(const QString &ip, int port);
    Q_INVOKABLE void removeTarget(const QString &ip, int port);

    // Single message control
    Q_INVOKABLE void startSendMessage(Message* message);
    Q_INVOKABLE void stopSendMessage(Message* message);

    // Group message control
    Q_INVOKABLE void startSendGroupMessage();
    Q_INVOKABLE void stopSendGroupMessage();
    Q_INVOKABLE void sendGroupMessageFollowOder();

    // Properties
    int intervalGroupMs() const;
    void setIntervalGroupMs(int newIntervalGroupMs);

signals:
    void intervalGroupMsChanged();

private:
    MessageManager* m_messageManager = nullptr;
    MulticastSender* m_multicastSender = nullptr;
    int m_intervalGroupMs = 0;
    SendWorkerPool* m_sendWorkerPool;
};

#endif // MESSAGECONTROLLER_H
