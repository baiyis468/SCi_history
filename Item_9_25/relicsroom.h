#ifndef RELICSROOM_H
#define RELICSROOM_H

#include "cloudRoom.h"
class Relics;
class relicsRoom : public CloudRoom
{
    ~relicsRoom();
public:
    const void* getData()const override;       //获取data
    static relicsRoom& getInstance();
public:
    bool addTheData(QString id,void* item,bool send=true)override;
    bool deleTheData(QString id,bool send=true)override;
    bool modifTheData(QString id,void* item,bool send=true)override;
    void* findTheData(QString id)override;
private slots:
    void UpdateData(uint8_t new_num)override;//对data进行更新
    void sendCloud() override;
    //以上是基类相关
    QMap<QString,Relics*>* data = nullptr;
};

#endif // RELICSROOM_H
