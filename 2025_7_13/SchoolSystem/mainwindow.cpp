#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QButtonGroup>
#include <QDebug>  // 用于调试输出
#include<QLabel>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化按钮组
    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);  // 单选模式
    buttonGroup->addButton(ui->ButtonClassSchedule, 0);
    buttonGroup->addButton(ui->ButtonMap, 1);
    buttonGroup->addButton(ui->ButtonToday, 2);
    buttonGroup->addButton(ui->ButtonNotice, 3);

    // 只创建已实现的ClassSchedule页面
    classScheduleWidget = new ClassSchedule(this);
    ui->stackedWidget->addWidget(classScheduleWidget); // 第0页

    // 为其他页面添加空占位
    for (int i = 1; i <= 3; i++) {
        QWidget* placeholder = new QWidget(this);
        placeholder->setStyleSheet("background-color: white;");
        ui->stackedWidget->addWidget(placeholder);
    }

    // 连接按钮点击到页面切换
    connect(buttonGroup, &QButtonGroup::idClicked, [this](int id) {
        if (id == 0) {
            ui->stackedWidget->setCurrentIndex(0);
        } else {
            // 其他页面显示提示信息
            qDebug() << "功能开发中，敬请期待！";

            // 在实际应用中，可以显示一个提示标签
            QLabel* label = new QLabel("🚧 功能开发中，敬请期待！", ui->stackedWidget->currentWidget());
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet("font-size: 20px; color: gray;");
            label->resize(ui->stackedWidget->size());
            label->show();
        }
    });

    // 默认显示课表页
    ui->stackedWidget->setCurrentIndex(0);
    ui->ButtonClassSchedule->setChecked(true);  // 设置初始选中状态
}

MainWindow::~MainWindow()
{
    delete ui;
}
