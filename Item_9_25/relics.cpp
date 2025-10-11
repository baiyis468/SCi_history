#include "relics.h"
#include<QJsonObject>
Relics::Relics(QObject *parent)
    : QObject{parent}
{}

QJsonObject Relics::getAll()
{
    QJsonObject obj;
    return {};//获取全部。还没做呢
}
