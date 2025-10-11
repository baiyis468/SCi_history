#ifndef ADMINI_H
#define ADMINI_H
#include <QObject>
class Admini: public QObject
{
    Q_OBJECT
public:
    Admini(QString ID,QString name,QByteArray password);
    QByteArray getPSWD();
private:
    QString m_ID;
    QString m_name;
    QByteArray m_password;
};

#endif // ADMINI_H
