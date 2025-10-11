#include "todaywidget.h"
#include "ui_todaywidget.h"
#include "databasemanager.h"
#include<QDateTime>
#include<QTimer>
TodayWidget::TodayWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TodayWidget)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    timer_2 = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&TodayWidget::UpdateTime);
    connect(timer_2,&QTimer::timeout,this,&TodayWidget::GetPeopleOnline);
    timer->start(1000);
    timer_2->start(1000);
    UpdateTime();
    GetPeopleOnline();

}

TodayWidget::~TodayWidget()
{
    delete ui;
}

void TodayWidget::UpdateTime()
{
    ui->label->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd\nhh:mm:ss"));
}

void TodayWidget::GetPeopleOnline()
{
    ui->labelOnline->setText("在线人数："+DatabaseManager::instance()->getOnlineString());
}
