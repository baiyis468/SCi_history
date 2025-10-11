#include "mytablemodel.h"
#include<QColor>

MyTableModel::MyTableModel(QObject *parent) : QAbstractTableModel(parent) {}

int MyTableModel::rowCount(const QModelIndex &parent) const    {    return 10;    }

int MyTableModel::columnCount(const QModelIndex &parent) const {    return 10;    }

QVariant MyTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::BackgroundRole)
    {
        // 如果是位置(1,1)，返回黄色背景
        if (index.row() == 1 && index.column() == 1) {
            return QColor(Qt::yellow);
        }
        // 其他单元格返回灰色背景
        return QColor(Qt::lightGray);
    }
    if (role == Qt::DisplayRole)
    {
        return QString("位置： %1, %2").arg(index.row()).arg(index.column());
    }
    return QVariant();
}

QVariant MyTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        return QString("表头 %1").arg(section);
    }
    return QVariant();
}
