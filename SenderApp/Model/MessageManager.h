#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <QAbstractListModel>
#include <QObject>
#include "Message.h"

class MessageManager : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role {
        IdMsg = Qt::UserRole + 1,
        ContentRole,
        IntervalRole
    };
    Q_ENUM(Role)

    explicit MessageManager(QObject *parent = nullptr);
    ~MessageManager();

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Quản lý Messages
    Q_INVOKABLE void addMessage();
    Q_INVOKABLE void deleteMessage(int idMsg);
    Q_INVOKABLE Message* getMessage(int idMsg) const;
    Q_INVOKABLE int messageCount() const;
    QList<Message*> getAllMessages() const;

    // Cập nhật dữ liệu
    void updateContent(int idMsg, const QString &content);
    void updateInterval(int idMsg, int intervalMs);

private:
    QList<Message*> m_messages;
    int m_nextId = 0;
};

#endif // MESSAGEMANAGER_H
