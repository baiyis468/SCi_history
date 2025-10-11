#include "settingwidget.h"
#include "ui_settingwidget.h"
#include "databasemanager.h"
#include "localcachemanager.h"
#include <QDate>
#include <QDebug>
#include<QPointer>
SettingWidget::SettingWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingWidget)
{
    ui->setupUi(this);
    ui->spinBox->setValue(LocalCacheManager::getInstance()->getToDays());

    // 连接错误信号
    DatabaseManager* databasemanager = DatabaseManager::instance();
    connect(databasemanager, &DatabaseManager::queryError, this, &SettingWidget::onQueryError);

    // 显示加载中提示
    ui->StartDateLabel->setText("加载中...");

    // 异步获取起始日期
    QPointer<SettingWidget> safeThis(this);
    databasemanager->getStartDate([safeThis](const QDate& startDate) {
        if(safeThis)safeThis->onStartDateReceived(startDate);
    });

    QPalette pal = ui->WarningLabel->palette();
    pal.setColor(QPalette::WindowText, Qt::red);   // 文字色
    ui->WarningLabel->setPalette(pal);
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget::onStartDateReceived(const QDate& startDate)
{
    if (startDate.isValid()) {
        ui->StartDateLabel->setText("数据库第1周星期一是：" + startDate.toString("yyyy年MM月dd日"));
    } else {
        ui->StartDateLabel->setText("获取起始日期失败");
    }
}

void SettingWidget::onQueryError(const QString& error)
{
    qDebug() << "设置页面查询错误:" << error;
    ui->StartDateLabel->setText("获取起始日期时发生错误");
}

void SettingWidget::on_ApplyButton_clicked()
{
    LocalCacheManager::getInstance()->updateToDays(ui->spinBox->value());
}
