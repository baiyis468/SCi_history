#ifndef ROUNDCORNERDELEGATE_H
#define ROUNDCORNERDELEGATE_H

#include <QStyledItemDelegate>

class RoundCornerDelegate : public QStyledItemDelegate
{
public:
    RoundCornerDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // ROUNDCORNERDELEGATE_H
