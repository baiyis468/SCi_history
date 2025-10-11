#include "adminiroom.h"
#include<QMap>
#include<admini.h>
#include<QJsonObject>
#include<QJsonDocument>
#include<QJsonValue>
#include<QJsonArray>
adminiRoom::~adminiRoom()
{
    for (auto&[x,y]: data->toStdMap())
    {
        delete y;  // 释放每个指针指向的内容
    }
    data->clear();
}

const void *adminiRoom::getData() const  { return data; }

adminiRoom &adminiRoom::getInstance()
{
    static adminiRoom instance;
    instance.data = new QMap<QString,Admini*>();
    QJsonDocument doc ;//应该从云端获取。
    if(doc.isObject())
    {
        QJsonObject obj = doc.object();
        instance.setNum(static_cast<uint8_t>(obj.value("num").toInt()));
        obj = obj.value("data").toObject();
        //这里遍历object的元素。这里的元素应该是admini类。new出来，放进去
        //最后这里的obj 相当于存的那根指针。
    }
    else
    {
        //云端出故障了！
    }
    return instance;
}
bool adminiRoom::addTheData(QString id, void *item,bool send)
{
    Admini* admini = dynamic_cast<Admini*>(static_cast<Admini*>(item));
    if(admini && !data->contains(id))
    {
        data->insert(id,admini);
        sendCloud();
        return true;
    }
    return false;
}

bool adminiRoom::deleTheData(QString id,bool send)
{
    if(data->contains(id))
    {
        data->remove(id);
        sendCloud();
        return true;
    }
    return false;
}

bool adminiRoom::modifTheData(QString id, void *item,bool send)
{
    Admini* admini = dynamic_cast<Admini*>(static_cast<Admini*>(item));
    if(admini && data->contains(id))
    {
        (*data)[id] = admini;
        sendCloud();
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


void adminiRoom::UpdateData(uint8_t new_num)
{
    QJsonDocument doc_num ;//应该从云端获取。
    if(doc_num.isObject())
    {
        QJsonObject objNum = doc_num.object();
        for(;getDataNum()!=new_num;changeNum())
        {
            QJsonArray change = objNum.value(QString::number(changeNum(getDataNum()))).toArray();//得到变化内容
            QString action = change[0].toString();
            QJsonArray y = change[1].toArray();
            if(action=="添加")
            {
                Admini* admini = new Admini;
                //Admini* admini = new Admini(y[1],y[2]);…………
                addTheData(y[0].toString(),admini,false);
            }
            else if(action=="删除")
            {
                deleTheData(y[0].toString(),false);
            }
            else if(action=="修改")
            {
                Admini* admini = new Admini;
                //Admini* admini = new Admini(y[1],y[2]);…………
                modifTheData(y[0].toString(),admini,false);
                //将y封装成QString admini* 调用修改函数。
            }
        }
    }
    else
    {
        //云端出问题了
    }
}

void adminiRoom::sendCloud()                                // 今天就做到这里啦！！！！！！！！！！！！
{
    // 不仅仅需要发给修改戳json ， 还得发给 data库。
    changeNum();
    //发给修改戳json
    QJsonObject MapObject;
    QJsonArray changeObject;

    QJsonArray changeData;//他对应admini ，如果增加就把属性都写下来。如果删除就只提供一个编号……

    changeData.append("属性1");
    changeData.append("属性2");
    changeData.append("属性3");
    changeObject.append("添加/删除/修改");
    changeObject.append(changeData);
    MapObject[QString::number(getDataNum())] = changeObject;

}
