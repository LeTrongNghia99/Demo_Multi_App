#include "Customfield.h"

CustomField::CustomField(const QString &key, const QString &value, QObject *parent)
    : m_key(key) , m_value(value) , QObject{parent}
{}
