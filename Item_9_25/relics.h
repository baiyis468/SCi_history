#ifndef RELICS_H
#define RELICS_H

#include <QObject>
class QJsonObject;
class Relics : public QObject
{
    Q_OBJECT
public:
    explicit Relics(QObject *parent = nullptr);
    QJsonObject getAll(); //把所有属性拿出来。

signals:
};

#endif // RELICS_H
