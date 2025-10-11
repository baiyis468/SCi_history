#ifndef ADMINI_H
#define ADMINI_H

#include <QObject>

class Admini : public QObject
{
    Q_OBJECT
public:
    explicit Admini(QObject *parent = nullptr);
private:

signals:
};

#endif // ADMINI_H
