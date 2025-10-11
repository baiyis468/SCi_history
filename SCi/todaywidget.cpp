#include "todaywidget.h"
#include "ui_todaywidget.h"
#include "databasemanager.h"
#include "colorpicker.h"
#include <QDate>
#include <QTimer>
#include <QDebug>

TodayWidget::TodayWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TodayWidget)
{
    ui->setupUi(this);

    // 连接错误信号
    DatabaseManager* dbManager = DatabaseManager::instance();
    connect(dbManager, &DatabaseManager::queryError, this, &TodayWidget::onQueryError);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TodayWidget::GetPeopleOnline);
    timer->start(100000);

    UpdateTime();
    GetPeopleOnline();

    // 异步获取当天课程表
    dbManager->getCurrDayClassSchedule([this](const QStringList& classSchedule) {
        onClassScheduleReceived(classSchedule);
    });
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
    // 这个方法现在由 onClassScheduleReceived 替代
}

void TodayWidget::GetPeopleOnline()
{
    // 异步获取在线人数
    DatabaseManager::instance()->getOnlineString([this](const QString& onlineCount) {
        onOnlineCountReceived(onlineCount);
    });
}

void TodayWidget::onOnlineCountReceived(const QString& onlineCount)
{
    if (onlineCount == "0") {
        // 如果在线人数为0，可能是查询失败
        emit onlineQueryFailed();
    }
    ui->labelOnline->setText("活跃人数：" + onlineCount);
}

void TodayWidget::onClassScheduleReceived(const QStringList& strlist)
{
    // 确保有5个结果（对应5节课）
    QStringList classSchedule = strlist;
    if (classSchedule.size() < 5) {
        classSchedule.resize(5);
    }

    for (int i = 0; i < 5; ++i) {
        // 动态获取 frame 和 flabel
        QString frameName = QString("frame_%1").arg(i+1);
        QString labelName = QString("flabel_%1").arg(i+1);

        QFrame* frame = findChild<QFrame*>(frameName);
        QLabel* label = findChild<QLabel*>(labelName);

        if (frame && label) {
            label->setText(classSchedule[i]);
            if (!classSchedule[i].isEmpty()) {
                QColor bgColor = ColorPicker::colorFor(classSchedule[i]);
                QPalette palette = frame->palette();
                palette.setColor(QPalette::Window, bgColor);
                frame->setPalette(palette);
                frame->setAutoFillBackground(true);

                palette = label->palette();
                palette.setColor(QPalette::WindowText, ColorPicker::getContrastTextColor(bgColor));
                label->setPalette(palette);
            } else {
                // 清空背景色
                QPalette palette = frame->palette();
                palette.setColor(QPalette::Window, Qt::white);
                frame->setPalette(palette);
                frame->setAutoFillBackground(true);
            }
        }
    }
}

void TodayWidget::onQueryError(const QString& error)
{
    qDebug() << "今日页面查询错误:" << error;
    emit onlineQueryFailed();
}
