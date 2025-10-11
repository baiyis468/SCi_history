#include "adminiroom.h"
#include<admini.h>
#include<QUrl>
#include<QtNetwork/QNetworkRequest>
#include<QtNetwork/QNetworkAccessManager>
#include<QEventLoop>
#include<QtNetwork/QRestReply>
#include<QDebug>
adminiRoom::adminiRoom()
{
    QNetworkAccessManager manager;
    QUrl url("https://jiabohaoshiyong.oss-cn-beijing.aliyuncs.com/%E7%94%A8%E6%88%B7%E4%BF%A1%E6%81%AF.txt?Expires=1726313441&OSSAccessKeyId=TMP.3KfULvvtwhfjhJkzMcjgJkZ9JWtmtk5HHfyCFn8WBFYsyFPgK1m4q3m6xbkv7wFLuRW15ctarL3boSGUMy5Cp8NMh1dRdF&Signature=1cyIhtPyl2Rt%2B7In6E0URANRFtA%3D&response-content-type=text%2Fplain%3Bcharset%3Dutf-8%3B");
    QNetworkRequest request(url);
    QNetworkReply* reply = manager.get(request);
    // 使用事件循环等待回复完成
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    // 读取回复内容
    QByteArray data = reply->readAll();
    QString content = QString::fromUtf8(data);
    qDebug()<<content;
    // 清理
    reply->deleteLater();
    QStringList strlist;
    QString str;
    for(auto&x:content)
    {
        if(x=='&'||x=='-')
        {
            strlist<<str;
            str.clear();
        }
        else str+=x;
    }
    for (int i = 0; i < strlist.size(); i+=3)
    {
        Admini* adminior = new Admini(strlist[i],strlist[i+1],strlist[i+2].toLocal8Bit());
        credentials.insert(strlist[i],adminior);
    }
}

Admini *adminiRoom::FindAdmini(QString name, QByteArray m_password)
{
    if(!credentials.contains(name)||credentials[name]->getPSWD()!=m_password)return nullptr;
    return credentials[name];
}
adminiRoom::~adminiRoom()
{
    for(auto&x:credentials)
    {
        delete x;
    }
}
