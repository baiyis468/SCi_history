#include "mytablemodel.h"
#include<QColor>

MyTableModel::MyTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    for(int i =0;i<rowCount();++i)
    {
        for(int j =0;j<columnCount();++j)
        {
            map.insert(qMakePair(i,j),qMakePair(Qt::lightGray,QStringList()));
        }
    }
}

int MyTableModel::rowCount(const QModelIndex &parent) const    {    return 5;    }

int MyTableModel::columnCount(const QModelIndex &parent) const {    return 7;    }

QVariant MyTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::BackgroundRole)
    {
        return QColor(map[qMakePair(index.row(),index.column())].first);
    }
    if (role == Qt::DisplayRole)
    {
        return QString("位置： %1, %2").arg(index.row()).arg(index.column());
    }
    return QVariant();
}
QVariant MyTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            QStringList daysOfWeek = {"周一", "周二", "周三", "周四", "周五", "周六", "周日"};
            if (section >= 0 && section < daysOfWeek.size()) return daysOfWeek[section];
        }
        else if (orientation == Qt::Vertical)
        {
            QStringList timesOfDay = {"上午1", "上午2", "下午1", "下午2", "晚课"};
            if (section >= 0 && section < timesOfDay.size()) return timesOfDay[section];
        }
    }
    return QVariant();
}
void MyTableModel::reset()//重置表格
{
    beginResetModel();

    for (int i = 0; i < rowCount(); ++i)
    {
        for (int j = 0; j < columnCount(); ++j)
        {
            map.remove(qMakePair(i, j));
        }
    }

    endResetModel();
}

void MyTableModel::updateTableModel()//更新表格
{
    beginResetModel();

    for (int i = 0; i < rowCount(); ++i)
    {
        for (int j = 0; j < columnCount(); ++j)
        {
            if (map.contains(qMakePair(i, j)))
            {
                auto value = map[qMakePair(i, j)];//好像在这里用不到
                emit dataChanged(index(i, j), index(i, j), {Qt::BackgroundRole, Qt::DisplayRole});
            }
            else
            {
                map.insert(qMakePair(i, j), qMakePair(Qt::lightGray, QStringList()));
                emit dataChanged(index(i, j), index(i, j), {Qt::BackgroundRole, Qt::DisplayRole});//通知让其重新获取颜色和文本。
            }
        }
    }

    endResetModel();
}

void MyTableModel::updateModel(QMap<QPair<int, int>, QPair<QColor, QStringList> > m)
{
    map = m;//这里有待改进！
    reset();
    updateTableModel();
}
