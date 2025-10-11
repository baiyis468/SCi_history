#include "classschedule.h"
#include "ui_classschedule.h"
#include "localcachemanager.h"
#include "databasemanager.h"
#include <QItemDelegate>
#include <QStandardItemModel>
#include <QDebug>

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

    dataBase = DatabaseManager::instance(this);
    connect(dataBase, &DatabaseManager::queryError, this, &ClassSchedule::onQueryError);

    delegate = new QItemDelegate(ui->tableView);
    ui->tableView->setItemDelegate(delegate);
    DataModel = localCache->getCoursesTableModel();
    ui->tableView->setModel(DataModel);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 从缓存中加载初始值
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

    // 异步获取专业列表
    dataBase->getClassSchedule([this](const QStringList& data) {
        onComboBox1DataReceived(data);
    });

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
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ClassSchedule::onCellClicked);
}

ClassSchedule::~ClassSchedule()
{
    delete ui;
}

void ClassSchedule::onComboBox1DataReceived(const QStringList& data)
{
    ui->comboBox_1->addItems(data);
    // 如果有缓存值，尝试选中它
    if (CacheMap.contains("major") && !CacheMap["major"].toString().isEmpty()) {
        int index = ui->comboBox_1->findText(CacheMap["major"].toString());
        if (index >= 0) {
            ui->comboBox_1->setCurrentIndex(index);
        }
    }
}

void ClassSchedule::onComboBox2DataReceived(const QStringList& data)
{
    ui->comboBox_2->addItems(data);
    ui->comboBox_2->setEnabled(true);

    // 恢复之前的选择或使用默认选择
    int index = data.contains(currentCombo2Text) ? ui->comboBox_2->findText(currentCombo2Text) : 0;
    ui->comboBox_2->setCurrentIndex(index);
}

void ClassSchedule::onComboBox3DataReceived(const QStringList& data)
{
    ui->comboBox_3->addItems(data);
    ui->comboBox_3->setEnabled(true);

    int index = data.contains(currentCombo3Text) ? ui->comboBox_3->findText(currentCombo3Text) : 0;
    ui->comboBox_3->setCurrentIndex(index);
}

void ClassSchedule::onComboBox4DataReceived(const QStringList& data)
{
    ui->comboBox_4->addItems(data);
    ui->comboBox_4->setEnabled(true);

    int index = data.contains(currentCombo4Text) ? ui->comboBox_4->findText(currentCombo4Text) : 0;
    ui->comboBox_4->setCurrentIndex(index);
}

void ClassSchedule::onComboBox5DataReceived(const QStringList& data)
{
    ui->comboBox_5->addItems(data);
    ui->comboBox_5->setEnabled(true);

    int index = data.contains(currentCombo5Text) ? ui->comboBox_5->findText(currentCombo5Text) : 0;
    ui->comboBox_5->setCurrentIndex(index);
}

void ClassSchedule::onClassScheduleModelReceived(QPair<bool, bool> result)
{
    cloudSuccess = result.first && cloudSuccess;
    cacheSuccess = result.second && cacheSuccess;

    // 从本地缓存获取模型并更新视图
    DataModel = localCache->getCoursesTableModel();
    ui->tableView->setModel(DataModel);
    ui->tableView->clearSelection();
    ui->labelCurrent->setText("当前未选中");
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ClassSchedule::onCellClicked);
}

void ClassSchedule::onQueryError(const QString& error)
{
    cloudSuccess = false;
    qDebug() << "云端查询失败：" << error;
}

void ClassSchedule::ComboBox1to2()
{
    QString str_last = ui->comboBox_1->currentText();
    currentCombo2Text = ui->comboBox_2->currentText();
    ui->comboBox_2->clear();

    if (!str_last.isEmpty()) {
        ui->comboBox_2->setEnabled(false);
        dataBase->getClassSchedule(str_last, [this](const QStringList& data) {
            onComboBox2DataReceived(data);
        });
    } else {
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
    currentCombo3Text = ui->comboBox_3->currentText();
    ui->comboBox_3->clear();

    if (!str_last.isEmpty()) {
        ui->comboBox_3->setEnabled(false);
        dataBase->getClassSchedule(ui->comboBox_1->currentText(), str_last,
                                   [this](const QStringList& data) {
                                       onComboBox3DataReceived(data);
                                   });
    } else {
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
    currentCombo4Text = ui->comboBox_4->currentText();
    ui->comboBox_4->clear();

    if (!str_last.isEmpty()) {
        ui->comboBox_4->setEnabled(false);
        dataBase->getClassSchedule(ui->comboBox_1->currentText(),
                                   ui->comboBox_2->currentText(),
                                   str_last,
                                   [this](const QStringList& data) {
                                       onComboBox4DataReceived(data);
                                   });
    } else {
        ui->comboBox_4->setEnabled(false);
        ui->comboBox_5->clear();
        ui->comboBox_5->setEnabled(false);
    }
}

void ClassSchedule::ComboBox4to5()
{
    QString str_last = ui->comboBox_4->currentText();
    currentCombo5Text = ui->comboBox_5->currentText();
    ui->comboBox_5->clear();

    if (!str_last.isEmpty()) {
        ui->comboBox_5->setEnabled(false);
        dataBase->getClassSchedule(ui->comboBox_1->currentText(),
                                   ui->comboBox_2->currentText(),
                                   ui->comboBox_3->currentText(),
                                   str_last,
                                   [this](const QStringList& data) {
                                       onComboBox5DataReceived(data);
                                   });
    } else {
        ui->comboBox_5->setEnabled(false);
    }
}

void ClassSchedule::ComboBox5toG()
{
    cloudSuccess = true;
    cacheSuccess = true;

    QString major = ui->comboBox_1->currentText();
    QString educationLevel = ui->comboBox_2->currentText();
    QString grade = ui->comboBox_3->currentText();
    QString classNum = ui->comboBox_4->currentText();
    int weekNum = ui->comboBox_5->currentText().toInt();

    // 更新缓存数据
    if (!localCache->updateCachingData(major, educationLevel, grade, classNum, weekNum, true)) {
        qDebug() << "缓存更新失败";
        cacheSuccess = false;
    }

    // 异步获取课程表模型
    dataBase->getClassScheduleModel(major, educationLevel, grade, classNum, weekNum,
                                    [this](QPair<bool, bool> result) {
                                        onClassScheduleModelReceived(result);
                                    });
}

void ClassSchedule::onCellClicked(const QModelIndex &current, const QModelIndex &previous)
{
    if (!current.isValid()) return;
    QString text = current.data(Qt::DisplayRole).toString();
    if (text.size()) {
        ui->labelCurrent->setText(text);
    } else {
        ui->labelCurrent->setText("当前位置无课程");
    }
}
