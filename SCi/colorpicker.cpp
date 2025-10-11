#include "colorpicker.h"
#include <QCryptographicHash>
#include <QList>
const QList<QColor> ColorPicker::colorTable = {
    QColor(255,  0,  0),   //  0° 红
    QColor(255, 44,  0),   // 10°
    QColor(255, 89,  0),   // 20°
    QColor(255,133,  0),   // 30°
    QColor(255,178,  0),   // 40°
    QColor(255,222,  0),   // 50°
    QColor(255,255,  0),   // 60° 黄
    QColor(211,255,  0),   // 70°
    QColor(167,255,  0),   // 80°
    QColor(122,255,  0),   // 90°
    QColor( 78,255,  0),   //100°
    QColor( 33,255,  0),   //110°
    QColor(  0,255, 22),   //120° 青绿
    QColor(  0,255, 67),   //130°
    QColor(  0,255,111),   //140°
    QColor(  0,255,156),   //150°
    QColor(  0,255,200),   //160°
    QColor(  0,255,244),   //170°
    QColor(  0,244,255),   //180° 青
    QColor(  0,200,255),   //190°
    QColor(  0,156,255),   //200°
    QColor(  0,111,255),   //210°
    QColor(  0, 67,255),   //220°
    QColor(  0, 22,255),   //230°
    QColor( 33,  0,255),   //240° 蓝
    QColor( 78,  0,255),   //250°
    QColor(122,  0,255),   //260°
    QColor(167,  0,255),   //270°
    QColor(211,  0,255),   //280°
    QColor(255,  0,255),   //290° 紫
    QColor(255,  0,222),   //300°
    QColor(255,  0,178),   //310°
    QColor(255,  0,133),   //320°
    QColor(255,  0, 89),   //330°
    QColor(255,  0, 44),   //340°
};
QColor ColorPicker::colorFor(const QString &key)
{
    if (key.isEmpty()) return Qt::gray;

    QByteArray hash = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5);
    quint32 hashValue = *reinterpret_cast<const quint32*>(hash.constData());
    int index = qAbs(static_cast<int>(hashValue)) % colorTable.size();
    return colorTable[index];
}

QColor ColorPicker::getContrastTextColor(const QColor color)
{
    return qGray(color.rgb()) > 128 ? Qt::black : Qt::white;
}
