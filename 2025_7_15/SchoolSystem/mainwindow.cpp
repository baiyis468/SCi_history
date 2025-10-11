#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QButtonGroup>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化按钮组
    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);  // 单选模式
    buttonGroup->addButton(ui->ButtonToday, 0);
    buttonGroup->addButton(ui->ButtonClassSchedule, 1);
    buttonGroup->addButton(ui->ButtonMap, 2);
    buttonGroup->addButton(ui->ButtonNotice, 3);

    // 添加页面到stackedWidget，按用户要求今日页作为索引0

    // 0: 今日页（占位） - 用户要求作为索引0
    QWidget* todayPlaceholder = createPlaceholderPage("🚧 今日功能开发中，敬请期待！");
    ui->stackedWidget->addWidget(todayPlaceholder); // 索引0

    // 1: 课表页面
    classScheduleWidget = new ClassSchedule(this);
    ui->stackedWidget->addWidget(classScheduleWidget); // 索引1

    // 2: 地图页（占位）
    QWidget* mapPlaceholder = createPlaceholderPage("🚧 地图功能开发中，敬请期待！");
    ui->stackedWidget->addWidget(mapPlaceholder); // 索引2

    // 3: 公告页面
    noticeWidget = new NoticeWidget(this);
    ui->stackedWidget->addWidget(noticeWidget); // 索引3

    // 连接按钮点击到页面切换
    connect(buttonGroup, &QButtonGroup::idClicked, this, [this](int id){
        // 按钮ID直接对应页面索引
        ui->stackedWidget->setCurrentIndex(id);
    });

    // 默认显示今日页（索引0）
    ui->stackedWidget->setCurrentIndex(0);
    ui->ButtonToday->setChecked(true);  // 设置今日按钮为默认选中
}

// 创建带提示标签的占位页
QWidget* MainWindow::createPlaceholderPage(const QString& message)
{
    QWidget* placeholder = new QWidget(this);
    placeholder->setStyleSheet("background-color: white;");

    QLabel* label = new QLabel(message, placeholder);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 20px; color: gray;");

    // 使用布局使标签居中
    QVBoxLayout* layout = new QVBoxLayout(placeholder);
    layout->addWidget(label);

    return placeholder;
}

MainWindow::~MainWindow()
{
    delete ui;
}
