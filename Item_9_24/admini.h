#ifndef ADMINI_H
#define ADMINI_H

#include <QObject>

class Admini : public QObject
{
    Q_OBJECT
public:
    explicit Admini(QObject *parent = nullptr);
    QJsonObject getAll();    //把所有属性拿出来。
    QByteArray getPSWD();
private:
    QByteArray m_password;
signals:
};

#endif // ADMINI_H
