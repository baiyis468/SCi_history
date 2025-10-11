#include "relicsroom.h"
#include<relics.h>
#include<QMap>
relicsRoom &relicsRoom::getInstance()
{
    static relicsRoom instance;
    return instance;
}
const void *relicsRoom::getData() const
{
    return data;
}
bool relicsRoom::addTheData(QString id, void *item)
{
    Relics* relics = dynamic_cast<Relics*>(static_cast<Relics*>(item));
    if(relics && !data->contains(id))
    {
        data->insert(id,relics);
        return true;
    }
    return false;
}

bool relicsRoom::deleTheData(QString id)
{
    if(data->contains(id))
    {
        data->remove(id);
        return true;
    }
    return false;
}

bool relicsRoom::modifTheData(QString id, void *item)
{
    Relics* relics = dynamic_cast<Relics*>(static_cast<Relics*>(item));
    if(relics && data->contains(id))
    {
        (*data)[id] = relics;
        return true;
    }
    return false;
}

void* relicsRoom::findTheData(QString id)
{
    if(data->contains(id))
    {
        return data->value(id);
    }
    return nullptr;
}
void relicsRoom::UpdateData()
{
    for (auto&[x,y]: data->toStdMap())
    {
        delete y;  // 释放每个指针指向的内容
    }
    data->clear();
}
