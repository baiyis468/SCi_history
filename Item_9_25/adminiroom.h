#ifndef ADMINIROOM_H
#define ADMINIROOM_H

#include "cloudRoom.h"
class Admini;
class adminiRoom : public CloudRoom
{
    ~adminiRoom();
public:
    const void* getData()const override;       //获取data
    static adminiRoom& getInstance();
public:
    bool addTheData(QString id,void* item,bool send=true)override;
    bool deleTheData(QString id,bool send=true)override;
    bool modifTheData(QString id,void* item,bool send=true)override;
    void* findTheData(QString id)override;
private slots:
    void UpdateData(uint8_t new_num)override;//对data进行更新
    void sendCloud() override;
    //以上是基类相关
private:
    QMap<QString,Admini*>* data = nullptr;
};

#endif // ADMINIROOM_H
