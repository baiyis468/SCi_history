#include "admini.h"
#include<QJsonObject>
Admini::Admini(QObject *parent)
    : QObject{parent}
{}

QJsonObject Admini::getAll()
{
    QJsonObject obj;
    obj.insert("密码",getPSWD().toInt());
    return {};//还没有属性
}

QByteArray Admini::getPSWD()
{
    return m_password;
}
