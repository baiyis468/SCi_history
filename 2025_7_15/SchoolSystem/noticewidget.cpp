#include "noticewidget.h"
#include "ui_noticewidget.h"
#include "databasemanager.h"

NoticeWidget::NoticeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NoticeWidget)
{
    ui->setupUi(this);
    DatabaseManager* database = DatabaseManager::instance();
    ui->listWidget->addItems(database->getNoticeStringList());
}

NoticeWidget::~NoticeWidget()
{
    delete ui;
}
