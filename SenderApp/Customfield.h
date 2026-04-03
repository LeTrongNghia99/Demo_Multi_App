#ifndef CUSTOMFIELD_H
#define CUSTOMFIELD_H

#include <QObject>

class CustomField : public QObject
{
    Q_OBJECT
public:
    explicit CustomField(const QString& key = "", const QString& value = "",QObject *parent = nullptr);
    QString key() const { return m_key; }
    QString value() const { return m_value; }
    void setKey(const QString& key) { m_key = key; }
    void setValue(const QString& value) { m_value = value; }
signals:

private:
    QString m_key;
    QString m_value;
};

#endif // CUSTOMFIELD_H
