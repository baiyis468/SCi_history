#include "cloudRoom.h"
#include <QTimer>
#include<QDebug>
void CloudRoom::changeNum()
{
    dataNum = changeNum(getDataNum());
}

void CloudRoom::setNum(uint8_t num)
{
    dataNum = num;
}

uint8_t CloudRoom::getDataNum()
{
    return dataNum;
}

uint8_t CloudRoom::changeNum(uint8_t num)
{
    if(num!=255)return num+1;
    else return 1;
}
void CloudRoom::updateCloud()
{
    uint8_t new_dataNum = 0;
    //访问云端。给new_dataNum赋值。
    if(new_dataNum != getDataNum())
    {
        emit dataChange(new_dataNum);
    }
}

void CloudRoom::timeUpdate()
{
    emit cloudSignal();
}


CloudRoom::CloudRoom(QObject *parent)
    : QObject{parent}
{
    QTimer *timer = new QTimer(this);
    connect(this,&CloudRoom::cloudSignal,this,&CloudRoom::updateCloud);
    connect(this,&CloudRoom::dataChange,this,&CloudRoom::UpdateData);
    connect(timer,&QTimer::timeout, this, &CloudRoom::updateCloud);
    // 不仅仅是定时器可以让其检测，cloudSignal也可以让其检测，此信号是留给其他的接口
    timer->start(10000);
}
