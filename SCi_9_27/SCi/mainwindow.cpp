#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QButtonGroup>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QNetworkInformation>
#include <QStatusBar>
#include <QStackedWidget>
#include "localcachemanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化数据库管理器
    dbManager = DatabaseManager::instance();
    connect(dbManager, &DatabaseManager::connectionStatusChanged,
            this, &MainWindow::onDatabaseConnectionChanged);

    LocalCacheManager* cacheManager = LocalCacheManager::getInstance();
    if (!cacheManager->openDatabase()) {
        qDebug() << "本地缓存数据库初始化失败";
    } else {
        qDebug() << "本地缓存数据库已成功初始化";
    }

    // 初始化状态栏
    ui->statusBar->setStyleSheet("background-color: red; color: white;");

    // 初始化网络监测
    if (QNetworkInformation::loadBackendByFeatures(QNetworkInformation::Feature::Reachability)) {
        networkInfo = QNetworkInformation::instance();
        if (networkInfo) {
            connect(networkInfo, &QNetworkInformation::reachabilityChanged,
                    this, &MainWindow::onNetworkChanged);
            checkNetworkStatus();
        }
    } else {
        qDebug() << "无法加载网络信息后端";
        checkNetworkStatus(); // 手动检查
    }

    // 初始化按钮组
    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);  // 单选模式
    buttonGroup->addButton(ui->ButtonToday, 0);
    buttonGroup->addButton(ui->ButtonClassSchedule, 1);
    buttonGroup->addButton(ui->ButtonMap, 2);
    buttonGroup->addButton(ui->ButtonNotice, 3);
    buttonGroup->addButton(ui->FindButton,4);
    buttonGroup->addButton(ui->SettingButton,5);

    // 初始化各个页面
    todayWidget = new TodayWidget(this);
    ui->stackedWidget->addWidget(todayWidget); // 索引0

    classScheduleWidget = new ClassSchedule(this);
    ui->stackedWidget->addWidget(classScheduleWidget); // 索引1

    mapWidget = new MapWidget(this);
    ui->stackedWidget->addWidget(mapWidget); // 索引2

    noticeWidget = new NoticeWidget(this);
    ui->stackedWidget->addWidget(noticeWidget); // 索引3

    findWidget = new FindWidget(this);
    ui->stackedWidget->addWidget(findWidget);  //索引4

    settingWidget = new SettingWidget(this);
    ui->stackedWidget->addWidget(settingWidget);  //索引5

    // 连接按钮点击到页面切换
    connect(buttonGroup, &QButtonGroup::idClicked, this, [this](int id){
        // 按钮ID直接对应页面索引
        ui->stackedWidget->setCurrentIndex(id);
    });

    // 连接页面切换信号
    connect(ui->stackedWidget, &QStackedWidget::currentChanged,
            this, &MainWindow::onPageChanged);

    // 默认显示今日页（索引0）
    ui->stackedWidget->setCurrentIndex(0);
    ui->ButtonToday->setChecked(true);  // 设置今日按钮为默认选中

    // 连接错误信号
    if (todayWidget) {
        connect(todayWidget, &TodayWidget::onlineQueryFailed, this,
                [this](){
                    todayPageError = true;
                    if (ui->stackedWidget->currentIndex() == 0) {
                        updatestatusBar();
                    }
                });
    }

    if (noticeWidget) {
        connect(noticeWidget, &NoticeWidget::noticeQueryFailed, this, [this]() {
            noticePageError = true;
            if (ui->stackedWidget->currentIndex() == 3) {
                updatestatusBar();
            }
        });
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNetworkChanged(QNetworkInformation::Reachability reachability)
{
    checkNetworkStatus();
}


void MainWindow::onPageChanged(int index)
{
    // 重置页面错误状态
    todayPageError = false;
    schedulePageCloudError = false;
    schedulePageCacheError = false;
    noticePageError = false;

    // 检测当前页面错误状态
    if (index == 0) {   // 今日页面
        // 今日页面的错误状态通过信号处理
    } else if (index == 1) {   // 课表页面
        // 从课表窗口获取错误状态
        if (classScheduleWidget) {
            schedulePageCloudError = !classScheduleWidget->cloudDataSuccess();
            schedulePageCacheError = !classScheduleWidget->cacheDataSuccess();
        }
    } else if (index == 3) {   // 公告页面
        // 公告页面的错误状态通过信号处理
    }
    updatestatusBar();
}

void MainWindow::updatestatusBar()
{
    QString message;

    // 网络未连接时优先显示网络错误
    if (!hasNetworkConnection) {
        message = "网络未连接";
        ui->statusBar->setStyleSheet("background-color: red; color: white;");
        ui->statusBar->showMessage(message);
        return;
    }

    // 数据库连接失败
    if (!databaseConnected) {
        message = "数据库连接失败";
        ui->statusBar->setStyleSheet("background-color: red; color: white;");
        ui->statusBar->showMessage(message);
        return;
    }

    // 当前页面有错误时显示页面特定错误
    int currentIndex = ui->stackedWidget->currentIndex();
    if (currentIndex == 0 && todayPageError) {
        message = "今日课程查询失败";
    } else if (currentIndex == 1) {
        if (schedulePageCloudError && schedulePageCacheError) {
            message = "课程表数据获取失败";
        } else if (schedulePageCloudError) {
            message = "云端课程表获取失败";
        } else if (schedulePageCacheError) {
            message = "缓存课程表获取失败";
        }
    } else if (currentIndex == 3 && noticePageError) {
        message = "公告获取失败";
    }

    // 更新状态栏
    if (!message.isEmpty()) {
        ui->statusBar->setStyleSheet("background-color: orange; color: white;"); // 改为橙色警告
        ui->statusBar->showMessage(message);
    } else {
        ui->statusBar->setStyleSheet("background-color: green; color: white;");
        ui->statusBar->showMessage("就绪");
    }
}

void MainWindow::checkNetworkStatus()
{
    if (networkInfo) {
        QNetworkInformation::Reachability status = networkInfo->reachability();
        hasNetworkConnection = (status == QNetworkInformation::Reachability::Online);
    }

    if (!hasNetworkConnection) {
        currentNetworkError = "网络未连接";
    } else {
        currentNetworkError.clear();
    }

    updatestatusBar();
}

void MainWindow::onDatabaseConnectionChanged(bool connected)
{
    databaseConnected = connected;
    QTimer::singleShot(500, this, &MainWindow::updatestatusBar);
}
