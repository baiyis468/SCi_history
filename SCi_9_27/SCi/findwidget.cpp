#include "findwidget.h"
#include "ui_findwidget.h"
#include "databasemanager.h"
#include <QListWidgetItem>
#include <QFont>
#include <QDebug>
#include<QPointer>

FindWidget::FindWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FindWidget)
{
    ui->setupUi(this);

    // 连接错误信号
    DatabaseManager* dbManager = DatabaseManager::instance();
    connect(dbManager, &DatabaseManager::queryError, this, &FindWidget::onQueryError);
    QPointer<FindWidget> safeThis(this);
    // 异步获取区域数据
    dbManager->getAreaItems([safeThis](const QStringList& areas) {
        if(safeThis)safeThis->onAreaItemsReceived(areas);
    });
}

FindWidget::~FindWidget()
{
    delete ui;
}

void FindWidget::onAreaItemsReceived(const QStringList& areas)
{
    ui->areaComboBox->clear();
    ui->areaComboBox->addItems(areas);

    // 如果有区域数据，自动触发获取建筑数据
    if (!areas.isEmpty()) {
        on_areaComboBox_currentTextChanged(areas.first());
    }
}

void FindWidget::onBuildingItemsReceived(const QStringList& buildings)
{
    ui->buildingComboBox->clear();
    ui->buildingComboBox->addItems(buildings);
}

void FindWidget::onEmptyRoomResultsReceived(const QStringList& rooms)
{
    ui->listWidget->clear();
    for (const auto& x : rooms) {
        QListWidgetItem *item = new QListWidgetItem(x);
        QFont font = item->font();
        font.setPixelSize(20);
        item->setFont(font);
        ui->listWidget->addItem(item);
    }
}

void FindWidget::onQueryError(const QString& error)
{
    qDebug() << "查询错误:" << error;
    // 可以在这里添加错误提示，例如显示一个消息框或状态栏消息
}

void FindWidget::on_pushButton_clicked()
{
    ui->listWidget->clear();
    ui->listWidget->addItem("查询中...");

    DatabaseManager* dbManager = DatabaseManager::instance();
    QPointer<FindWidget> safeThis(this);

    dbManager->getEmptyRoom(
        ui->areaComboBox->currentText(),
        ui->buildingComboBox->currentText(),
        ui->weekSpinBox->value(),
        ui->daySpinBox->value(),
        [safeThis](const QStringList& rooms) {if(safeThis)safeThis->onEmptyRoomResultsReceived(rooms);}
        );
}
void FindWidget::on_areaComboBox_currentTextChanged(const QString &arg1)
{
    if (arg1.isEmpty())
    {
        ui->buildingComboBox->clear();
        return;
    }

    ui->buildingComboBox->clear();
    ui->buildingComboBox->addItem("加载中...");
    QPointer<FindWidget> safeThis(this);
    DatabaseManager* dbManager = DatabaseManager::instance();
    dbManager->getBuildingItems(arg1, [safeThis](const QStringList& buildings) {
        if(safeThis)safeThis->onBuildingItemsReceived(buildings);
    });
}
