#include "adminiroom.h"
#include<QMap>
#include<admini.h>
const void *adminiRoom::getData() const  { return data; }

adminiRoom &adminiRoom::getInstance()
{
    static adminiRoom instance;
    return instance;
}

bool adminiRoom::addTheData(QString id, void *item)
{
    Admini* admini = dynamic_cast<Admini*>(static_cast<Admini*>(item));
    if(admini && !data->contains(id))
    {
        data->insert(id,admini);
        return true;
    }
    return false;
}

bool adminiRoom::deleTheData(QString id)
{
    if(data->contains(id))
    {
        data->remove(id);
        return true;
    }
    return false;
}

bool adminiRoom::modifTheData(QString id, void *item)
{
    Admini* admini = dynamic_cast<Admini*>(static_cast<Admini*>(item));
    if(admini && data->contains(id))
    {
        (*data)[id] = admini;
        return true;
    }
    return false;
}

void* adminiRoom::findTheData(QString id)
{
    if(data->contains(id))
    {
        return data->value(id);
    }
    return nullptr;
}


void adminiRoom::UpdateData()
{
    for (auto&[x,y]: data->toStdMap())
    {
        delete y;  // 释放每个指针指向的内容
    }
    data->clear();
    //获取新的data，并进行替换

}
