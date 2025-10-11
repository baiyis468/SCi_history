#include "mainwindow.h"
#include <QApplication>
#include "databasemanager.h"
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置应用程序响应性检查
    QTimer::singleShot(30000, []() {
        qDebug() << "应用程序运行30秒，检查是否无响应...";
    });

    MainWindow w;
    w.show();
    int ret = a.exec();

    // 等待退出操作完成，设置超时
    QEventLoop loop;
    QTimer::singleShot(3000, &loop, &QEventLoop::quit);
    DatabaseManager::instance()->prepareForExit();
    loop.exec();

    return ret;
}
