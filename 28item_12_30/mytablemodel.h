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
private:
    void reset();
    void updateTableModel();
public:
    void updateModel(QMap<QPair<int,int>,QPair<QColor,QStringList>> m);         //我要更新map，然后分别调用reset和updateTableModel来完成视图的更新。

private:
    QMap<QPair<int,int>,QPair<QColor,QStringList>> map;

};

#endif // MYTABLEMODEL_H
