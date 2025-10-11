#include "relicsroom.h"
#include<relics.h>
#include<QMap>
#include<QJsonObject>
#include<QJsonValue>
#include<QJsonDocument>
relicsRoom &relicsRoom::getInstance()
{
    static relicsRoom instance;
    instance.data = new QMap<QString,Relics*>();
    return instance;
}
relicsRoom::~relicsRoom()
{
    for (auto&[x,y]: data->toStdMap())
    {
        delete y;  // 释放每个指针指向的内容
    }
    data->clear();
}

const void* relicsRoom::getData() const
{
    return data;
}
bool relicsRoom::addTheData(QString id, void *item,bool send)
{
    Relics* relics = dynamic_cast<Relics*>(static_cast<Relics*>(item));
    if(relics && !data->contains(id))
    {
        data->insert(id,relics);
        sendCloud();
        return true;
    }
    return false;
}

bool relicsRoom::deleTheData(QString id,bool send)
{
    if(data->contains(id))
    {
        data->remove(id);
        sendCloud();
        return true;
    }
    return false;
}

bool relicsRoom::modifTheData(QString id, void *item,bool send)
{
    Relics* relics = dynamic_cast<Relics*>(static_cast<Relics*>(item));
    if(relics && data->contains(id))
    {
        (*data)[id] = relics;
        sendCloud();
        return true;
    }
    return false;
}

void* relicsRoom::findTheData(QString id)
{
    if(data->contains(id))  return data->value(id);
    return nullptr;
}
void relicsRoom::UpdateData(uint8_t new_num)
{
}

void relicsRoom::sendCloud()
{

}
