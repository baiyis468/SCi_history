#ifndef CLOUDROOM_H
#define CLOUDROOM_H

#include <QObject>
class Admini;
class CloudRoom : public QObject
{
    Q_OBJECT


public:
    static CloudRoom& getInstance();
protected://设置为protected 使其派生类可继承。
    explicit CloudRoom(QObject *parent = nullptr);
private:
    CloudRoom(const CloudRoom&) = delete;
    CloudRoom& operator=(const CloudRoom&) = delete;

//  单例模式。



public:
    void changeNum ();
    void setNum(uint8_t num);

    //更新修改戳

public:
    virtual bool addTheData(QString id,void* item)   = 0;
    virtual bool deleTheData(QString id)  = 0;
    virtual bool modifTheData(QString id,void* item) = 0;
    virtual void* findTheData(QString id)  = 0;

    // 增删改查

public:
    virtual const void* getData()const = 0;
    uint8_t getDataNum();

    //获取函数



signals:
    void dataChange();//data更改信号
    void cloudSignal();//云端检测信号
    //云端检测。改变发信号dataChange
public:
    void timeUpdate(); //定时器，定时发送云端检测信号。

private slots:
    void updateCloud();             //云端检测。
    virtual void UpdateData() = 0;  //本地更新data
    virtual void sendCloud() = 0;    // 我给云端的响应函数 我向云端发送内容
private:
    uint8_t dataNum = 0;
};

#endif // CLOUDROOM_H
