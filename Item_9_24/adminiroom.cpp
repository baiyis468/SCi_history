#include "adminiroom.h"
#include<QMap>
#include<admini.h>
#include<QJsonObject>
#include<QJsonDocument>
const void *adminiRoom::getData() const  { return data; }

adminiRoom &adminiRoom::getInstance()
{
    static adminiRoom instance;
    instance.data = new QMap<QString,Admini*>();
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


void adminiRoom::UpdateData()               //从云端获取要更改，我希望num跟data存在连个不同的json里面。****
{
    if(data)
    {
        for (auto&[x,y]: data->toStdMap())
        {
            delete y;  // 释放每个指针指向的内容
        }
        data->clear();
    }
    //获取新的data，并进行替换
    QJsonDocument doc ;//应该从云端获取。
    if(doc.isObject())
    {
        QJsonObject obj = doc.object();
        setNum(static_cast<uint8_t>(obj.value("num").toInt()));
        obj = obj.value("data").toObject();
        //这里遍历object的元素。
    }
    else
    {
        //云端出故障了！
    }
}

void adminiRoom::sendCloud()
{
    changeNum();
    QJsonObject obj;
    obj.insert("num",getDataNum());
    QJsonObject objMap;
    for(auto&[x,y]:data->toStdMap())
    {
        objMap.insert(x,y->getAll());
    }
    obj.insert("data",objMap);
    //此时将其发送给云端，让云端接收。
}
