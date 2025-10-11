#ifndef COLORPICKER_H
#define COLORPICKER_H
#include <QColor>
#include <QString>
class ColorPicker
{
public:
    static QColor colorFor(const QString &key);
    static QColor getContrastTextColor(const QColor color);
private:
    static const QList<QColor> colorTable;
};

#endif // COLORPICKER_H
