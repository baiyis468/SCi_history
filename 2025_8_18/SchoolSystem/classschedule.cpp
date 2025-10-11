#include "classschedule.h"
#include "ui_classschedule.h"
#include"localcachemanager.h"
#include"databasemanager.h"
#include<QItemDelegate>
#include<QSqlQueryModel>
#include <QStandardItemModel>
ClassSchedule::ClassSchedule(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClassSchedule)
{
    ui->setupUi(this);
    localCache = LocalCacheManager::getInstance();
    if (!localCache->openDatabase())
    {
        qDebug() << "缓存数据库打开失败!";
        cacheSuccess = false;
    }
    dataBase = DatabaseManager::instance();
    delegate = new QItemDelegate(ui->tableView);
    ui->tableView->setItemDelegate(delegate);
    DataModel = localCache->getCoursesTableModel();
    ui->tableView->setModel(DataModel);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);//禁止多选
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);//禁止整行整列选
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 将其填入combobox，combobox的当前项。
    CacheMap = localCache->getCachingDataMap();
    if (CacheMap.contains("major") && !CacheMap["major"].toString().isEmpty())
    ui->comboBox_1->addItem(CacheMap["major"].toString());
    if (CacheMap.contains("education_level") && !CacheMap["education_level"].toString().isEmpty())
    ui->comboBox_2->addItem(CacheMap["education_level"].toString());
    if (CacheMap.contains("grade") && !CacheMap["grade"].toString().isEmpty())
    ui->comboBox_3->addItem(CacheMap["grade"].toString());
    if (CacheMap.contains("class_num") && !CacheMap["class_num"].toString().isEmpty())
    ui->comboBox_4->addItem(CacheMap["class_num"].toString());
    if (CacheMap.contains("week_num") && !CacheMap["week_num"].toString().isEmpty())
    ui->comboBox_5->addItem(CacheMap["week_num"].toString());

    ui->comboBox_1->addItems(dataBase->getClassSchedule());   //导致缓存的那个major重复出现。 但也是为了第一次使用正确初始化
    ComboBox1to2();ComboBox2to3();ComboBox3to4();ComboBox4to5();// 似乎这几个是多余的？不知道这时候combobox_1有没有跟connect绑定好。

    connect(ui->comboBox_1, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ClassSchedule::ComboBox1to2);
    connect(ui->comboBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ClassSchedule::ComboBox2to3);
    connect(ui->comboBox_3, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ClassSchedule::ComboBox3to4);
    connect(ui->comboBox_4, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ClassSchedule::ComboBox4to5);
    connect(ui->comboBox_5, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ClassSchedule::ComboBox5toG);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentChanged,this, &ClassSchedule::onCellClicked);
    connect(dataBase, &DatabaseManager::queryError, this, [this](const QString& err){cloudSuccess = false;qDebug() << "云端查询失败：" << err;});
}
ClassSchedule::~ClassSchedule(){delete ui;}
void ClassSchedule::ComboBox1to2()
{
    QString str_last = ui->comboBox_1->currentText();
    QString str_temp = ui->comboBox_2->currentText();
    ui->comboBox_2->clear();
    if(!str_last.isEmpty())// 两个有效检查  还差一个检查数据源的有效性 majorLevels.contains(major)
    {
        QStringList strlist = dataBase->getClassSchedule(ui->comboBox_1->currentText());// 存放 2 的项目
        ui->comboBox_2->addItems(strlist);
        ui->comboBox_2->setEnabled(true);
        int index = strlist.contains(str_temp)?ui->comboBox_2->findText(str_temp):0;
        ui->comboBox_2->setCurrentIndex(index);
    }
    else
    {
        ui->comboBox_2->setEnabled(false);
        ui->comboBox_3->clear();
        ui->comboBox_3->setEnabled(false);
        ui->comboBox_4->clear();
        ui->comboBox_4->setEnabled(false);
        ui->comboBox_5->clear();
        ui->comboBox_5->setEnabled(false);
    }
}

void ClassSchedule::ComboBox2to3()
{
    QString str_last = ui->comboBox_2->currentText();
    QString str_temp = ui->comboBox_3->currentText();
    ui->comboBox_3->clear();
    if(!str_last.isEmpty())
    {
        QStringList strlist = dataBase->getClassSchedule(ui->comboBox_1->currentText(),ui->comboBox_2->currentText());
        ui->comboBox_3->addItems(strlist);
        ui->comboBox_3->setEnabled(true);
        int index = strlist.contains(str_temp) ? ui->comboBox_3->findText(str_temp) : 0;
        ui->comboBox_3->setCurrentIndex(index);
    }
    else
    {
        ui->comboBox_3->setEnabled(false);
        ui->comboBox_4->clear();
        ui->comboBox_4->setEnabled(false);
        ui->comboBox_5->clear();
        ui->comboBox_5->setEnabled(false);
    }

}

void ClassSchedule::ComboBox3to4()
{
    QString str_last = ui->comboBox_3->currentText();
    QString str_temp = ui->comboBox_4->currentText();
    ui->comboBox_4->clear();
    if(!str_last.isEmpty())
    {
        QStringList strlist = dataBase->getClassSchedule(ui->comboBox_1->currentText(),ui->comboBox_2->currentText(),ui->comboBox_3->currentText());
        ui->comboBox_4->addItems(strlist);
        ui->comboBox_4->setEnabled(true);
        int index = strlist.contains(str_temp) ? ui->comboBox_4->findText(str_temp):0;
        ui->comboBox_4->setCurrentIndex(index);
    }
    else
    {
        ui->comboBox_4->setEnabled(false);
        ui->comboBox_5->clear();
        ui->comboBox_5->setEnabled(false);
    }
}

void ClassSchedule::ComboBox4to5()
{
    QString str_last = ui->comboBox_4->currentText();
    QString str_temp = ui->comboBox_5->currentText();
    ui->comboBox_5->clear();
    if(!str_last.isEmpty())
    {
        QStringList strlist = dataBase->getClassSchedule(ui->comboBox_1->currentText(),ui->comboBox_2->currentText(),ui->comboBox_3->currentText(),ui->comboBox_4->currentText());
        ui->comboBox_5->addItems(strlist);
        ui->comboBox_5->setEnabled(true);
        int index = strlist.contains(str_temp)?ui->comboBox_5->findText(str_temp):0;
        ui->comboBox_5->setCurrentIndex(index);
    }
    else
    {
        ui->comboBox_5->setEnabled(false);
    }
}
void ClassSchedule::ComboBox5toG()
{
    cloudSuccess = true;
    cacheSuccess = true;

    // 更新缓存数据
    if (!localCache->updateCachingData(ui->comboBox_1->currentText(),ui->comboBox_2->currentText(),ui->comboBox_3->currentText(),ui->comboBox_4->currentText(),ui->comboBox_5->currentText().toInt(),true))
    {
        qDebug() << "缓存更新失败";
        cacheSuccess = false;
    }
    QPair<bool,bool> pa = dataBase->getClassScheduleModel(ui->comboBox_1->currentText(),ui->comboBox_2->currentText(),ui->comboBox_3->currentText(),ui->comboBox_4->currentText(),ui->comboBox_5->currentText().toInt());
    cloudSuccess = pa.first && cloudSuccess;
    cacheSuccess = pa.second && cacheSuccess;
    // 从本地缓存获取模型并更新视图
    DataModel = localCache->getCoursesTableModel();
    ui->tableView->setModel(DataModel);
    ui->tableView->clearSelection();
    ui->labelCurrent->setText("当前未选中");
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentChanged,this, &ClassSchedule::onCellClicked);
}
void ClassSchedule::onCellClicked(const QModelIndex &current, const QModelIndex &previous)
{
    if(!current.isValid()) return;
    QString text = current.data(Qt::DisplayRole).toString();
    if(text.size())ui->labelCurrent->setText(text);
    else ui->labelCurrent->setText("当前位置无课程");
}
