#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <QObject>

class Observable : public QObject
{
    Q_OBJECT
public:
    explicit Observable(QObject *parent = nullptr);

signals:
};

#endif // OBSERVABLE_H
