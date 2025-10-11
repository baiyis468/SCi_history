#include "findwidget.h"
#include "ui_findwidget.h"
#include"databasemanager.h"
FindWidget::FindWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FindWidget)
{
    ui->setupUi(this);
    ui->areaComboBox->addItems(DatabaseManager::instance()->getAreaItems());
}

FindWidget::~FindWidget()
{
    delete ui;
}

void FindWidget::on_pushButton_clicked()
{
    ui->listWidget->clear();
    QStringList list = DatabaseManager::instance()->getEmptyRoom(ui->areaComboBox->currentText(),ui->buildingComboBox->currentText(),ui->weekSpinBox->value(),ui->daySpinBox->value());
    for(auto&x:list)
    {
        QListWidgetItem *item = new QListWidgetItem(x);
        QFont font = item->font();
        font.setPixelSize(20);
        item->setFont(font);
        ui->listWidget->addItem(item);
    }
}


void FindWidget::on_areaComboBox_currentTextChanged(const QString &arg1)
{
    ui->buildingComboBox->clear();
    ui->buildingComboBox->addItems(DatabaseManager::instance()->getBuildingItems(arg1));
}

