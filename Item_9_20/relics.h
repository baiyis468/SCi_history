#ifndef RELICS_H
#define RELICS_H

#include <QObject>

class Relics : public QObject
{
    Q_OBJECT
public:
    explicit Relics(QObject *parent = nullptr);

signals:
};

#endif // RELICS_H
