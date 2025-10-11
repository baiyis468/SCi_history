#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

#include <QAbstractTableModel>
class MyTableModel : public QAbstractTableModel
{
public:
    MyTableModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};

#endif // MYTABLEMODEL_H
