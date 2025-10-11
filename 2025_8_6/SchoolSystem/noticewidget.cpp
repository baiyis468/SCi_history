#include "noticewidget.h"
#include "ui_noticewidget.h"
#include "databasemanager.h"

// noticewidget.cpp
NoticeWidget::NoticeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NoticeWidget)
{
    ui->setupUi(this);
    DatabaseManager* database = DatabaseManager::instance();
    QStringList notices = database->getNoticeStringList();

    if (notices.isEmpty()) {
        emit noticeQueryFailed();
    }

    ui->listWidget->addItems(notices);
}

NoticeWidget::~NoticeWidget()
{
    delete ui;
}
