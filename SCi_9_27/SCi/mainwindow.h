#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkInformation>
#include "classschedule.h"
#include "noticewidget.h"
#include "todaywidget.h"
#include "mapwidget.h"
#include "findwidget.h"
#include "settingwidget.h"
#include "databasemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ClassSchedule* classScheduleWidget = nullptr;
    NoticeWidget* noticeWidget = nullptr;
    TodayWidget* todayWidget = nullptr;
    MapWidget* mapWidget = nullptr;
    FindWidget* findWidget = nullptr;
    SettingWidget* settingWidget = nullptr;
    DatabaseManager* dbManager = nullptr;

    QNetworkInformation *networkInfo = nullptr;
    QString currentNetworkError;
    bool hasNetworkConnection = false;
    bool databaseConnected = false;

    // 添加页面错误状态
    bool todayPageError = false;
    bool schedulePageCloudError = false;
    bool schedulePageCacheError = false;
    bool noticePageError = false;

    void updatestatusBar();          // 更新状态栏
    void checkNetworkStatus();       // 检查网络状态

private slots:
    void onPageChanged(int index); // 页面切换时的处理
    void onNetworkChanged(QNetworkInformation::Reachability reachability); // 网络状态变化
    void onDatabaseConnectionChanged(bool connected); // 数据库连接状态变化
};

#endif // MAINWINDOW_H
