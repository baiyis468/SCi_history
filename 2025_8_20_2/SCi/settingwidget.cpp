#include "settingwidget.h"
#include "ui_settingwidget.h"
#include"databasemanager.h"
#include"localcachemanager.h"
#include<QDate>
SettingWidget::SettingWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingWidget)
{
    ui->setupUi(this);
    ui->spinBox->setValue(LocalCacheManager::getInstance()->getToDays());
    DatabaseManager* databasemanager = DatabaseManager::instance();
    ui->StartDateLabel->setText("数据库第1周星期一是："+databasemanager->getStartDate().toString("yyyy年MM月dd日"));
    QPalette pal = ui->WarningLabel->palette();
    pal.setColor(QPalette::WindowText, Qt::red);   // 文字色
    ui->WarningLabel->setPalette(pal);
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget::on_ApplyButton_clicked()
{
    LocalCacheManager::getInstance()->updateToDays(ui->spinBox->value());
}

