#include "noticewidget.h"
#include "ui_noticewidget.h"
#include "databasemanager.h"
#include <QDebug>
#include<QPointer>
NoticeWidget::NoticeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NoticeWidget)
{
    ui->setupUi(this);

    // 连接错误信号
    DatabaseManager* database = DatabaseManager::instance();
    connect(database, &DatabaseManager::queryError, this, &NoticeWidget::onQueryError);

    // 显示加载中提示
    ui->listWidget->addItem("加载中...");

    // 异步获取公告数据
    QPointer<NoticeWidget> safeThis(this);
    database->getNoticeStringList([safeThis](const QStringList& notices) {
        if(safeThis)safeThis->onNoticesReceived(notices);
    });
}

NoticeWidget::~NoticeWidget()
{
    delete ui;
}

void NoticeWidget::onNoticesReceived(const QStringList& notices)
{
    ui->listWidget->clear();

    if (notices.isEmpty()) {
        emit noticeQueryFailed();
        ui->listWidget->addItem("暂无公告");
        return;
    }

    ui->listWidget->addItems(notices);
}

void NoticeWidget::onQueryError(const QString& error)
{
    qDebug() << "公告查询错误:" << error;
    ui->listWidget->clear();
    ui->listWidget->addItem("加载失败: " + error);
    emit noticeQueryFailed();
}
