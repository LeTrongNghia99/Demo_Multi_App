#ifndef CUSTOMFIELDMODEL_H
#define CUSTOMFIELDMODEL_H
#include"Customfield.h"
#include <QAbstractListModel>

class CustomFieldModel : public QAbstractListModel
{
    Q_OBJECT // Thêm dòng này nếu dùng Qt6
public:
    enum FieldRoles {
        KeyRole = Qt::UserRole + 1,
        ValueRole
    };
    Q_ENUM(FieldRoles)

    explicit CustomFieldModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addField(const QString& key, const QString& value);
    Q_INVOKABLE void updateField(int index, const QString& key, const QString& value);
    Q_INVOKABLE void updateKey(int index, const QString& key);
    Q_INVOKABLE void updateValue(int index, const QString& value);
    Q_INVOKABLE void removeField(int index);

    QList<CustomField*> fields() const { return m_fields; }
    void clear();

private:
    QList<CustomField*> m_fields;
};

#endif // CUSTOMFIELDMODEL_H
