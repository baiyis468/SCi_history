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
const void* relicsRoom::getData() const
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
    if(data->contains(id))  return data->value(id);
    return nullptr;
}
void relicsRoom::UpdateData()
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

void relicsRoom::sendCloud()
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
