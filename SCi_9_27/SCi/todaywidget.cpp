#include "todaywidget.h"
#include "ui_todaywidget.h"
#include "databasemanager.h"
#include "colorpicker.h"
#include<QThread>
#include <QDate>
#include <QTimer>
#include <QDebug>
#include<QPointer>

TodayWidget::TodayWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TodayWidget)
{
    qDebug() << "=== TodayWidget 构造函数开始 ===";

    ui->setupUi(this);
    qDebug() << "UI setup 完成";

    // 验证所有UI组件都存在
    for (int i = 1; i <= 5; ++i) {
        QString frameName = QString("frame_%1").arg(i);
        QString labelName = QString("flabel_%1").arg(i);

        QFrame* frame = findChild<QFrame*>(frameName);
        QLabel* label = findChild<QLabel*>(labelName);

        if (frame && label) {
            qDebug() << "找到组件:" << frameName << "和" << labelName;
            label->setText("加载中...");
        } else {
            qDebug() << "警告: 未找到组件:" << frameName << "或" << labelName;
        }
    }

    // 连接错误信号
    DatabaseManager* dbManager = DatabaseManager::instance();
    if (dbManager) {
        connect(dbManager, &DatabaseManager::queryError, this, &TodayWidget::onQueryError);
        qDebug() << "数据库管理器连接成功";
    } else {
        qDebug() << "错误: 数据库管理器实例为null";
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TodayWidget::GetPeopleOnline);
    timer->start(100000);

    UpdateTime();
    GetPeopleOnline();

    // 异步获取当天课程表
    if (dbManager) {
        QPointer<TodayWidget> safeThis(this);
        dbManager->getCurrDayClassSchedule([safeThis](const QStringList& classSchedule) {
            if(safeThis) {
                qDebug() << "准备调用 onClassScheduleReceived";
                safeThis->onClassScheduleReceived(classSchedule);
            } else {
                qDebug() << "安全指针失效，跳过回调";
            }
        });
        qDebug() << "已发起课程表查询请求";
    }

    qDebug() << "=== TodayWidget 构造函数结束 ===";
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
    QPointer<TodayWidget> safeThis(this);
    DatabaseManager::instance()->getOnlineString([safeThis](const QString& onlineCount) {
        if(safeThis)safeThis->onOnlineCountReceived(onlineCount);
    });
}

void TodayWidget::onOnlineCountReceived(const QString& onlineCount)
{
    qDebug() << "onOnlineCountReceived调用时，传入参数:" << onlineCount;

    // 在线人数为0是正常情况，不应该视为错误
    // 只有当查询失败（返回空字符串或无效值）时才认为是错误
    if (onlineCount.isEmpty() || onlineCount == "-1" || onlineCount == "error") {
        qDebug() << "在线人数查询失败";
        emit onlineQueryFailed();
        ui->labelOnline->setText("活跃人数：查询失败");
    } else {
        // 即使在线人数为0也是正常情况
        ui->labelOnline->setText("活跃人数：" + onlineCount);
        qDebug() << "在线人数显示成功:" << onlineCount;
    }
    update();
}

void TodayWidget::onClassScheduleReceived(const QStringList& strlist)
{
    qDebug() << "=== onClassScheduleReceived 开始执行 ===";
    qDebug() << "strlist 大小:" << strlist.size();

    // 安全检查
    if (!ui) {
        qDebug() << "UI指针无效";
        return;
    }

    // 检查是否在主线程，如果不是则切换到主线程
    if (QThread::currentThread() != this->thread()) {
        qDebug() << "不在主线程，使用invokeMethod切换到主线程";
        QMetaObject::invokeMethod(this, "onClassScheduleReceived",
                                  Qt::QueuedConnection,
                                  Q_ARG(QStringList, strlist));
        return;
    }

    // 检查数据是否有效
    bool hasValidData = false;
    for (int i = 0; i < strlist.size() && i < 5; ++i) {
        if (!strlist[i].isEmpty() && strlist[i] != "无课程") {
            hasValidData = true;
            break;
        }
    }

    if (!hasValidData) {
        qDebug() << "没有有效的课程数据，可能是缓存数据未设置";
        // 这不算错误，只是没有数据
    }

    // 设置课程显示
    for (int i = 0; i < 5; ++i) {
        QString labelName = QString("flabel_%1").arg(i+1);
        QLabel* label = findChild<QLabel*>(labelName);

        if (label) {
            QString text;
            if (i < strlist.size() && !strlist[i].isEmpty()) {
                text = strlist[i];
            } else {
                text = "无课程";
            }
            label->setText(text);
            qDebug() << "设置" << labelName << "为:" << text;

            // 设置背景色 - 使用ColorPicker进行配色
            QString frameName = QString("frame_%1").arg(i+1);
            QFrame* frame = findChild<QFrame*>(frameName);
            if (frame) {
                if (text != "无课程" && text != "加载中...") {
                    // 使用ColorPicker根据课程名称生成颜色
                    QColor bgColor = ColorPicker::colorFor(text.split(" ").first()); // 只使用课程名部分
                    QColor textColor = ColorPicker::getContrastTextColor(bgColor);

                    QString styleSheet = QString(
                                             "background-color: %1; "
                                             "color: %2; "
                                             "border-radius: 8px; "
                                             "padding: 5px;"
                                             ).arg(bgColor.name(), textColor.name());

                    frame->setStyleSheet(styleSheet);
                    label->setStyleSheet(QString("color: %1;").arg(textColor.name()));
                } else {
                    // 无课程或加载中的默认样式
                    frame->setStyleSheet(
                        "background-color: white; "
                        "color: black; "
                        "border-radius: 8px; "
                        "padding: 5px;"
                        );
                    label->setStyleSheet("color: black;");
                }
            }
        }
    }

    qDebug() << "=== onClassScheduleReceived 执行完成 ===";
    update();
}

void TodayWidget::onQueryError(const QString& error)
{
    qDebug() << "今日页面查询错误:" << error;
    emit onlineQueryFailed();
}
