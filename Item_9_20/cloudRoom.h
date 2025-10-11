#ifndef CLOUDROOM_H
#define CLOUDROOM_H

#include <QObject>
class Admini;
class CloudRoom : public QObject
{
    Q_OBJECT
public:
    static CloudRoom& getInstance();          //拿出单例

public:
    virtual bool addTheData(QString id,void* item)   = 0;
    virtual bool deleTheData(QString id)  = 0;
    virtual bool modifTheData(QString id,void* item) = 0;
    virtual void* findTheData(QString id)  = 0;
    // 增删改查
    virtual const void* getData()const = 0;   //获取data
    uint8_t getDataNum();                     //获取修改戳
    void updateCloud();                      // 判断云端和现在是不是不一样了？不一样了就改变data发信号dataChange
private slots:
    virtual void UpdateData() = 0;
    void sendCloud();    // 我给云端的响应函数 我向云端发送内容
signals:
    void dataChange();
protected:                                  //设置为protected 使其派生类可继承。
    explicit CloudRoom(QObject *parent = nullptr);
private:
    CloudRoom(const CloudRoom&) = delete;
    CloudRoom& operator=(const CloudRoom&) = delete;
    uint8_t dataNum;
};

#endif // CLOUDROOM_H
