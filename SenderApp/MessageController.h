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
private:
    QList <Message*> m_messages;
    int m_id;
};

#endif // MESSAGECONTROLLER_H
