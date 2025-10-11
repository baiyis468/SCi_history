#include "roundcornerdelegate.h"
#include<QPainterPath>
#include<QPainter>
RoundCornerDelegate::RoundCornerDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void RoundCornerDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int radius = 10;  // 圆角半径
    QStyleOptionViewItem optionCopy = option;

    QColor bgColor = Qt::white;
    if (index.isValid())
    {
        bgColor = Qt::lightGray;
    }

    QPainterPath path;
    path.addRoundedRect(optionCopy.rect, radius, radius);
    painter->setClipPath(path);  // 设置裁剪区域为圆角矩形
    painter->fillPath(path, bgColor);  // 填充背景色

    // 绘制文本或其他内容
    QStyledItemDelegate::paint(painter, option, index);
}
