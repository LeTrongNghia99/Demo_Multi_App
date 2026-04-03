#pragma once
#include <QString>
#include <QJsonObject>
#include <QTimer>
#include "CustomFieldModel.h"
class Message : public QObject{
    Q_OBJECT
public:
    Message(const int msgId, const QString& content, int intervalMs ,QObject *parent = nullptr );

    Q_PROPERTY(CustomFieldModel* customFieldModel READ customFieldModel WRITE setCustomFieldModel NOTIFY customFieldModelChanged FINAL)
    int msgId() const;
    QString content() const;
    int intervalMs() const;
    QTimer* timerMs() const;



    void setContent(const QString& content);
    void setIntervalMs(int intervalMs);
    void setCustomFields(const QString& key,const QString& value);


    CustomFieldModel *customFieldModel() const;
    void setCustomFieldModel(CustomFieldModel *newCustomFieldModel);

signals:

    void customFieldModelChanged();

private:
    int m_msgId;
    QString m_content;
    int m_intervalMs;
    QTimer* m_timerMs;
    CustomFieldModel *m_customFieldModel = nullptr;
};
