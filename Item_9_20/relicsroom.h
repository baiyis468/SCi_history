#ifndef RELICSROOM_H
#define RELICSROOM_H

#include "cloudRoom.h"
class Relics;
class relicsRoom : public CloudRoom
{
public:
    const void* getData()const override;       //获取data
    static relicsRoom& getInstance();
public:
    bool addTheData(QString id,void* item)override;
    bool deleTheData(QString id)override;
    bool modifTheData(QString id,void* item)override;
    void* findTheData(QString id)override;
private slots:
    void UpdateData()override;//对data进行更新
    //以上是基类相关
    QMap<QString,Relics*>* data;
};

#endif // RELICSROOM_H
