#ifndef GETCURRICULUM_H
#define GETCURRICULUM_H
#include<QMultiMap>
#include <QMap>
#include <QString>
#include<QPair>
#include<QColor>
#include<QVector>
class getCurriculum
{
public:
    // 获取单例实例的方法
    static getCurriculum& getInstance();

    // 获取课程信息的 QMap
    QMap<QString, QMap<QString, QMultiMap<QString, QString>>> getMap();
    QVector<QMap<QPair<int,int>,QPair<QColor,QStringList>>> getwVector();

private:
    // 私有构造函数，防止外部直接创建对象
    getCurriculum();


    // 存储课程信息的 QMap
    static void convertFileEncoding(const QString &inputFilePath, const QString &outputFilePath);
    static QString convertFileEncoding(const QString &inputFilePath);
public:
    static void getCloudClass(const QString &fileName);
private:
    static QVector<QMap<QPair<int,int>,QPair<QColor,QStringList>>> wVector;// 里面一个qmap一个5*7
    static QString nowVectorFile;
    static QMap<QString, QMap<QString, QMultiMap<QString, QString>>> map;
    static getCurriculum* instance;

};

#endif // GETCURRICULUM_H
