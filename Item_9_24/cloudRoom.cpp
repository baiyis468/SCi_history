#include "cloudRoom.h"


void CloudRoom::changeNum()
{
    if(getDataNum()!=255) dataNum = getDataNum()+1;
    else dataNum = 1;
}

void CloudRoom::setNum(uint8_t num)
{
    dataNum = num;
}

uint8_t CloudRoom::getDataNum()
{
    return dataNum;
}
void CloudRoom::updateCloud()
{
    uint8_t new_dataNum = 0;
    //访问云端。给new_dataNum赋值。
    if(new_dataNum != getDataNum())
    {
        emit dataChange();
    }
}

void CloudRoom::timeUpdate()
{
    //定时。到时间就：
    emit cloudSignal();
}


CloudRoom::CloudRoom(QObject *parent)
    : QObject{parent}
{
    connect(this,&CloudRoom::cloudSignal,this,&CloudRoom::updateCloud);
    connect(this,&CloudRoom::dataChange,this,&CloudRoom::UpdateData);
}
