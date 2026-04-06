#pragma once
#include <QString>
#include <QJsonObject>
#include <QTimer>
#include "CustomFieldModel.h"
class Message : public QObject{
    Q_OBJECT
public:
    Message( QObject *parent = nullptr );
    int msgId() const;
    QString content() const;
    int intervalMs() const;
    QTimer* timerMs() const;
    CustomFieldModel *customFieldModel() const;

    void setContent(const QString& content);
    void setIntervalMs(int intervalMs);
    void setCustomFields(const QString& key,const QString& value);
    void setCustomFieldModel(CustomFieldModel *newCustomFieldModel);
    void setMsgId(int newMsgId);

private:
    Q_PROPERTY(CustomFieldModel* customFieldModel READ customFieldModel WRITE setCustomFieldModel NOTIFY customFieldModelChanged FINAL)
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged FINAL)
    Q_PROPERTY(int msgId READ msgId WRITE setMsgId NOTIFY msgIdChanged FINAL)
    Q_PROPERTY(int intervalMs READ intervalMs WRITE setIntervalMs NOTIFY intervalMsChanged FINAL)
    int m_msgId;
    QString m_content;
    int m_intervalMs;
    QTimer* m_timerMs;
    CustomFieldModel *m_customFieldModel = nullptr;

signals:
    void customFieldModelChanged();
    void contentChanged();
    void msgIdChanged();
    void intervalMsChanged();
};
