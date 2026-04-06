#ifndef MESSAGECONTROLLER_H
#define MESSAGECONTROLLER_H

#include "Message.h"
#include "MulticastSender.h"
#include <QAbstractListModel>
#include <QObject>

class MessageController : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role{
        IdMsg = Qt::UserRole + 1,
        ContentRole,
        IntervalRole
    };
    Q_ENUM(Role)

    MessageController(QObject* parent = nullptr);

    //QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void updateContent(int idMsg, const QString &content);
    Q_INVOKABLE void updateInterval(int idMsg, int intervalMs);
    Q_INVOKABLE void addMessage();
    Q_INVOKABLE void deleteMessage(int idMsg);
    Q_INVOKABLE void startSendMessage(const int &idMsg , MulticastSender *multicastSender );
    Q_INVOKABLE void stopSendMessage(const int &idMsg, MulticastSender *multicastSender);
    Q_INVOKABLE Message* getMessage(int idMsg);
    Q_INVOKABLE void startSendGroupMessage( MulticastSender *multicastSender);
    Q_INVOKABLE void stopSendGroupMessage( MulticastSender *multicastSender);
    Q_INVOKABLE void sendGroupMessageFollowOder(MulticastSender *multicastSender);

    int intervalGroupMs() const;
    void setIntervalGroupMs(int newIntervalGroupMs);

signals:
    void intervalGroupMsChanged();

private:
    Q_PROPERTY(int intervalGroupMs READ intervalGroupMs WRITE setIntervalGroupMs NOTIFY intervalGroupMsChanged FINAL)
    QList <Message*> m_messages;
    int m_id;
    int m_intervalGroupMs;
};

#endif // MESSAGECONTROLLER_H
