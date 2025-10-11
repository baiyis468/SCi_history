#include "mainwindow.h"
#include <QApplication>
#include"databasemanager.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    int ret = a.exec();

    // 程序退出前清理数据库
    DatabaseManager::instance()->prepareForExit();
    return ret;
}
