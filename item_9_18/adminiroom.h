#ifndef ADMINIROOM_H
#define ADMINIROOM_H
#include <QCryptographicHash>
#include <QHash>
#include <QObject>
class Admini;
class adminiRoom: public QObject
{
    Q_OBJECT
public:
    adminiRoom();//应该初始化管理员表。
    Admini* FindAdmini(QString name,QByteArray m_password);
    void Deletemini(QString name);
    ~adminiRoom();
private:
    QHash<QString, Admini*> credentials;// 一个账号对应一个管理员
};

#endif // ADMINIROOM_H
