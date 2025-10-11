#include "cloudRoom.h"


uint8_t CloudRoom::getDataNum()
{
    return dataNum;
}
void CloudRoom::updateCloud()
{
    uint8_t new_dataNum = 0;
    //定时器持续获取new_dataNum;
    if(new_dataNum != getDataNum())
    {
        emit dataChange();
        //对data进行更改。
    }
}

void CloudRoom::sendCloud()
{
    //打包成json，发送过去。注意修改戳传的时候要+1(或是重置为0)。
    //注意自身的修改戳也要更改，这样就不用重构了。
}

CloudRoom::CloudRoom(QObject *parent)
    : QObject{parent}
{
    connect(this,&CloudRoom::dataChange,this,&CloudRoom::UpdateData);
}
