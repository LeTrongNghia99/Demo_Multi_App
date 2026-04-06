#include "CustomFieldModel.h"

CustomFieldModel::CustomFieldModel(QObject* parent) : QAbstractListModel(parent) {}

int CustomFieldModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : m_fields.size();
}

QVariant CustomFieldModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_fields.size())
        return QVariant();
    CustomField* field = m_fields[index.row()];
    if (role == KeyRole) return field->key();
    if (role == ValueRole) return field->value();
    return QVariant();
}

QHash<int, QByteArray> CustomFieldModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[KeyRole] = "key";
    roles[ValueRole] = "value";
    return roles;
}

void CustomFieldModel::addField(const QString& key, const QString& value) {
    beginInsertRows(QModelIndex(), m_fields.size(), m_fields.size());
    m_fields.append(new CustomField(key, value));
    endInsertRows();
}

void CustomFieldModel::updateField(int index, const QString& key, const QString& value) {
    if (index < 0 || index >= m_fields.size()) return;
    m_fields[index]->setKey(key);
    m_fields[index]->setValue(value);
    emit dataChanged(this->index(index), this->index(index), {KeyRole, ValueRole});
}

void CustomFieldModel::updateKey(int index, const QString& key) {
    qDebug() << "[Sender App ] CustomFieldModel::updateKey ";
    if (index < 0 || index >= m_fields.size()) return;
    m_fields[index]->setKey(key);
    emit dataChanged(this->index(index), this->index(index));
}

void CustomFieldModel::updateValue(int index, const QString& value) {
    qDebug() << "[Sender App ] CustomFieldModel::updateValue ";
    if (index < 0 || index >= m_fields.size()) return;
    m_fields[index]->setValue(value);
    emit dataChanged(this->index(index), this->index(index));
}

void CustomFieldModel::removeField(int index) {
    if (index < 0 || index >= m_fields.size()) return;
    beginRemoveRows(QModelIndex(), index, index);
    delete m_fields.takeAt(index);
    endRemoveRows();
}

void CustomFieldModel::clear() {
    beginResetModel();
    qDeleteAll(m_fields);
    m_fields.clear();
    endResetModel();
}