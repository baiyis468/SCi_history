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
    UpdateClass();
}

TodayWidget::~TodayWidget()
{
    delete ui;
}

void TodayWidget::UpdateTime()
{
    ui->label->setText(QDate::currentDate().toString("yyyy-MM-dd"));
}

void TodayWidget::UpdateClass()
{
    QStringList strlist = DatabaseManager::instance()->getCurrDayClassSchedule();
    //QStringList strlist = {"","","","",""};
    // 确保有5个结果（对应5节课）
    if(strlist.size() < 5) {
        strlist.resize(5);
    }

    for(int i = 0; i < 5; ++i)
    {
        // 动态获取 frame 和 flabel
        QString frameName = QString("frame_%1").arg(i+1);
        QString labelName = QString("flabel_%1").arg(i+1);

        QFrame* frame = findChild<QFrame*>(frameName);
        QLabel* label = findChild<QLabel*>(labelName);

        label->setText(strlist[i]);
    }
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
