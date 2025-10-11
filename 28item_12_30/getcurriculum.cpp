#include "getcurriculum.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include<QStringList>
#include <QStringConverter>
#include<QString>
#include<QPair>
getCurriculum* getCurriculum::instance = nullptr;
QMap<QString, QMap<QString, QMultiMap<QString, QString>>> getCurriculum::map;

getCurriculum& getCurriculum::getInstance()
{
    if (instance == nullptr)
    {
        instance = new getCurriculum();
    }
    return *instance;
}
QVector<QMap<QPair<int, int>, QPair<QColor, QStringList>>> getCurriculum::wVector;
QString getCurriculum::nowVectorFile;


getCurriculum::getCurriculum()
{
    wVector.reserve(20);wVector.resize(20);

    QStringList strlist;
    convertFileEncoding("D:/shiyan/untitled28/人工智能/检索文件.txt","D:/shiyan/untitled28/人工智能/检索文件out.txt");
    QFile file("D:/shiyan/untitled28/人工智能/检索文件out.txt");

    // 尝试打开文件
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 读取文件内容
        QString content = file.readAll();
        file.close();
        QString str;
        for (auto& x : content)
        {
            if (x == '\n')
            {
                if (!str.isEmpty() && str.back() == ' ') str.chop(1);
                strlist.append(str);
                str.clear();
            } else
            {
                str += x;
            }
        }
    } else
    {
        qDebug() << "无法打开文件";
    }

    //qDebug()<<strlist;

    QString major, level;
    //QMap<QString, QMap<QString, QMultiMap<QString, QString>>>
    for (auto& str : strlist)
    {
        if (str[0] == '#' && str[1] == '#')
        {
            major = str.mid(2);
            map.insert(major,QMap<QString, QMultiMap<QString, QString>>());
        }
        else if (str[0] == '#' && str[1] == '$' && !major.isNull())
        {
            level = str.mid(2);
            map[major].insert(level,QMultiMap<QString, QString>());
        }
        else if (str[0] == '#' && str[1] == '%' && !major.isNull() && !level.isNull())
        {

            map[major][level].insert(str.mid(2,2),str.back());
        }
        else
        {
            qDebug() << "文件格式出错！:"<<str;
        }
    }
    //qDebug()<<map;
}

void getCurriculum::convertFileEncoding(const QString &inputFilePath, const QString &outputFilePath)
{
    QFile file(inputFilePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "无法打开文件" << inputFilePath;
        return;
    }

    // 读取文件内容为 QByteArray
    QByteArray encodedData = file.readAll();
    file.close();

    // 使用 QStringConverter 从 ANSI 编码转换为 QString (UTF-16)
    QString content = QString::fromLocal8Bit(encodedData);

    // 将 QString 转换为 UTF-8 编码的 QByteArray
    QByteArray utf8Data = content.toUtf8();

    // 将转换后的内容写入新文件
    QFile outputFile(outputFilePath);
    if(outputFile.exists())return;
    if (!outputFile.open(QIODevice::WriteOnly)) {
        qWarning() << "无法打开文件" << outputFilePath;
        return;
    }

    QTextStream out(&outputFile);
    // 直接写入 UTF-8 编码的数据
    out << utf8Data;
    outputFile.close();
}

QString getCurriculum::convertFileEncoding(const QString &inputFilePath)
{
    QFile file(inputFilePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "无法打开文件" << inputFilePath;
        return "";
    }

    // 读取文件内容为 QByteArray
    QByteArray encodedData = file.readAll();
    file.close();

    // 使用 QStringConverter 从 ANSI 编码转换为 QString (UTF-16)
    QString content = QString::fromLocal8Bit(encodedData);

    // 将 QString 转换为 UTF-8 编码的 QByteArray
    QByteArray utf8Data = content.toUtf8();
    return  QString::fromUtf8(utf8Data);

}
void getCurriculum::getCloudClass(const QString &fileName)
{
//////////////////////////////////
    QString str = convertFileEncoding("C:/Users/86135/Desktop/人工智能/_人工智能_本_22_1 .txt");
    QStringList strlist = str.split("\r\n", Qt::SkipEmptyParts);
////////////////////////////////////////////////////
    if(fileName==nowVectorFile) return;
    nowVectorFile = fileName;wVector.clear();wVector.reserve(20);wVector.resize(20);
    //向云端获取。
    strlist.pop_front();//不要第一行
    int j;QPair<int,int> pair;QVector<int> week;
    QString className, teacherName, placeName;

    for(int i =0;i<strlist.size();++i)
    {
        if (strlist[i][0] == '@' && strlist[i][1] == '@')
        {
            //qDebug()<<strlist[i];

            pair = qMakePair(strlist[i].mid(4).toInt(),strlist[i].mid(2,1).toInt());// j,i
        }
        else if (strlist[i][0] == '@' && strlist[i][1] == '#')
        {
            //qDebug()<<strlist[i];

            j = strlist[i++].mid(2,1).toInt();
            for(int k = 0; k < j; ++k,i+=6)
            {

                //qDebug()<<strlist[i];
                //for(int ii = 1;ii<=5;++ii)qDebug()<<strlist[ii+i];

                if(strlist[i][0] == '@' && strlist[i][1] == '$'&&
                    strlist[i+1][0] == '@' && strlist[i+1][1] == '%'&&
                    strlist[i+2][0] == '#' && strlist[i+2][1] == '@'&&
                    strlist[i+3][0] == '#' && strlist[i+3][1] == '#'&&
                    strlist[i+4][0] == '#' && strlist[i+4][1] == '$'&&
                    strlist[i+5][0] == '#' && strlist[i+5][1] == '%')
                {
                    QString str = strlist[i].mid(2);   //这里的问题。没有存对。
                    str.remove(0,1);str += '_';
                    QString s;
                    for(auto& c:str)
                    {
                        if(c=='_')
                        {
                            week.push_back(s.toInt());
                            s.clear();
                        }
                        else    s += c;
                    }
                    className = strlist[i+1].mid(2);
                    teacherName = strlist[i+2].mid(2);
                    placeName = strlist[i+3].mid(2)+"-"+strlist[i+4].mid(2)+"-"+strlist[i+5].mid(2);
                    qDebug()<<week;
                    for(auto&x:week)
                    {
                        wVector[x][pair] = qMakePair(Qt::blue,QStringList()<<className<<teacherName<<placeName);
                        //这里之后改一下，拿到新的课程名称时，就把颜色特定好，用到map，
                    }
                    week.clear();
                }
                else    qDebug()<<"格式有问题！";
            }
            --i;
        }
    }
}

// 获取课程信息的 QMap
QMap<QString, QMap<QString, QMultiMap<QString, QString>>> getCurriculum::getMap(){      return map;     }

QVector<QMap<QPair<int, int>, QPair<QColor, QStringList> > > getCurriculum::getwVector(){   return wVector;    }

