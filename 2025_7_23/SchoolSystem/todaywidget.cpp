#include "todaywidget.h"
#include "ui_todaywidget.h"
#include "databasemanager.h"
#include <QDate>
#include<QTimer>
TodayWidget::TodayWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TodayWidget)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TodayWidget::GetPeopleOnline);
    timer->start(1000);

    UpdateTime();
    GetPeopleOnline();
}

TodayWidget::~TodayWidget()
{
    delete ui;
}

void TodayWidget::UpdateTime()
{
    ui->label->setText(QDate::currentDate().toString("yyyy-MM-dd"));
}

void TodayWidget::GetPeopleOnline()
{
    QString onlineStr = DatabaseManager::instance()->getOnlineString();
    if (onlineStr == "0") {
        // 如果在线人数为0，可能是查询失败
        emit onlineQueryFailed();
    }
    ui->labelOnline->setText("在线人数：" + onlineStr);
}
