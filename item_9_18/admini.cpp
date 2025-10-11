#include "admini.h"

Admini::Admini(QString ID, QString name, QByteArray password):m_ID(ID),m_name(name),m_password(password){}

QByteArray Admini::getPSWD()
{
    return m_password;
}
