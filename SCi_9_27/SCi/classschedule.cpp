#include "classschedule.h"
#include "ui_classschedule.h"
#include "localcachemanager.h"
#include "databasemanager.h"
#include <QItemDelegate>
#include <QStandardItemModel>
#include <QDebug>
#include <QTimer>
#include <QPointer>

ClassSchedule::ClassSchedule(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClassSchedule)
{
    ui->setupUi(this);

    qDebug() << "ClassSchedule 构造函数开始";

    localCache = LocalCacheManager::getInstance();
    if (!localCache->openDatabase()) {
        qDebug() << "缓存数据库打开失败!";
        cacheSuccess = false;
    } else {
        qDebug() << "缓存数据库打开成功";
    }

    dataBase = DatabaseManager::instance(this);
    connect(dataBase, &DatabaseManager::queryError, this, &ClassSchedule::onQueryError);

    // 初始化UI
    delegate = new QItemDelegate(ui->tableView);
    ui->tableView->setItemDelegate(delegate);

    // 先设置一个空的模型
    DataModel = new QStandardItemModel(5, 7, this);
    QStringList horizontalHeaders = {"周一", "周二", "周三", "周四", "周五", "周六", "周日"};
    QStringList verticalHeaders = {"第一节", "第二节", "第三节", "第四节", "第五节"};
    DataModel->setHorizontalHeaderLabels(horizontalHeaders);
    DataModel->setVerticalHeaderLabels(verticalHeaders);
    ui->tableView->setModel(DataModel);

    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 从缓存中加载初始值
    CacheMap = localCache->getCachingDataMap();
    qDebug() << "缓存数据内容:";
    for (auto it = CacheMap.begin(); it != CacheMap.end(); ++it) {
        qDebug() << it.key() << ":" << it.value();
    }

    // 检查缓存数据是否有效
    bool cacheValid = !CacheMap.isEmpty() &&
                      CacheMap.contains("major") && !CacheMap["major"].toString().isEmpty() &&
                      CacheMap.contains("education_level") && !CacheMap["education_level"].toString().isEmpty() &&
                      CacheMap.contains("grade") && !CacheMap["grade"].toString().isEmpty() &&
                      CacheMap.contains("class_num") && !CacheMap["class_num"].toString().isEmpty() &&
                      CacheMap.contains("week_num") && CacheMap["week_num"].toInt() > 0;

    qDebug() << "缓存数据有效性:" << cacheValid;

    // 关键修改：无论缓存是否有效，都先设置缓存项，再异步加载完整列表
    ui->comboBox_1->clear();
    ui->comboBox_2->clear();
    ui->comboBox_3->clear();
    ui->comboBox_4->clear();
    ui->comboBox_5->clear();

    if (cacheValid) {
        // 先添加缓存项作为默认选项
        ui->comboBox_1->addItem(CacheMap["major"].toString());
        ui->comboBox_2->addItem(CacheMap["education_level"].toString());
        ui->comboBox_3->addItem(CacheMap["grade"].toString());
        ui->comboBox_4->addItem(CacheMap["class_num"].toString());
        ui->comboBox_5->addItem(CacheMap["week_num"].toString());

        qDebug() << "已设置缓存项为默认选项";
    } else {
        qDebug() << "缓存数据无效，需要用户手动选择";
    }

    // 关键修改：异步获取专业列表（无论缓存是否有效都要执行）
    dataBase->getClassSchedule([this](const QStringList& data) {
        onComboBox1DataReceived(data);
    });

    // 连接信号槽
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

    qDebug() << "ClassSchedule 构造函数结束";
}

ClassSchedule::~ClassSchedule()
{
    delete ui;
}

void ClassSchedule::onComboBox1DataReceived(const QStringList& data)
{
    // 保存当前的缓存项
    QString cacheMajor = CacheMap.contains("major") ? CacheMap["major"].toString() : "";

    // 先清空combobox，然后添加完整列表
    ui->comboBox_1->clear();
    ui->comboBox_1->addItems(data);

    // 关键修改：确保缓存项被选中（如果存在）
    if (!cacheMajor.isEmpty()) {
        int index = ui->comboBox_1->findText(cacheMajor);
        if (index >= 0) {
            ui->comboBox_1->setCurrentIndex(index);
            qDebug() << "已选中缓存专业:" << cacheMajor;
        } else {
            // 如果缓存项不在列表中，仍然添加到combobox并选中
            ui->comboBox_1->insertItem(0, cacheMajor);
            ui->comboBox_1->setCurrentIndex(0);
            qDebug() << "缓存专业不在列表中，已添加并选中:" << cacheMajor;
        }
    }

    // 延迟触发级联查询，确保UI更新完成
    QTimer::singleShot(100, this, [this]() {
        if (!ui->comboBox_1->currentText().isEmpty()) {
            ComboBox1to2();
        }
    });
}

void ClassSchedule::onComboBox2DataReceived(const QStringList& data)
{
    QString cacheEducationLevel = CacheMap.contains("education_level") ? CacheMap["education_level"].toString() : "";

    ui->comboBox_2->clear();
    ui->comboBox_2->addItems(data);
    ui->comboBox_2->setEnabled(true);

    if (!cacheEducationLevel.isEmpty()) {
        int index = ui->comboBox_2->findText(cacheEducationLevel);
        if (index >= 0) {
            ui->comboBox_2->setCurrentIndex(index);
        } else {
            ui->comboBox_2->insertItem(0, cacheEducationLevel);
            ui->comboBox_2->setCurrentIndex(0);
        }
    }

    // 继续级联查询
    QTimer::singleShot(100, this, [this]() {
        if (!ui->comboBox_2->currentText().isEmpty()) {
            ComboBox2to3();
        }
    });
}

void ClassSchedule::onComboBox3DataReceived(const QStringList& data)
{
    qDebug() << "=== onComboBox3DataReceived ===";
    qDebug() << "接收到届数据，数量:" << data.size();
    for (int i = 0; i < data.size(); ++i) {
        qDebug() << "数据" << i << ":" << data[i];
    }

    QString cacheGrade = CacheMap.contains("grade") ? CacheMap["grade"].toString() : "";

    ui->comboBox_3->clear();
    ui->comboBox_3->addItems(data);
    ui->comboBox_3->setEnabled(true);

    if (!cacheGrade.isEmpty()) {
        int index = ui->comboBox_3->findText(cacheGrade);
        if (index >= 0) {
            ui->comboBox_3->setCurrentIndex(index);
        } else {
            ui->comboBox_3->insertItem(0, cacheGrade);
            ui->comboBox_3->setCurrentIndex(0);
        }
    }

    qDebug() << "当前届combobox项目数:" << ui->comboBox_3->count();

    // 继续级联查询
    QTimer::singleShot(100, this, [this]() {
        if (!ui->comboBox_3->currentText().isEmpty()) {
            ComboBox3to4();
        }
    });
}

void ClassSchedule::onComboBox4DataReceived(const QStringList& data)
{
    QString cacheClassNum = CacheMap.contains("class_num") ? CacheMap["class_num"].toString() : "";

    ui->comboBox_4->clear();
    ui->comboBox_4->addItems(data);
    ui->comboBox_4->setEnabled(true);

    if (!cacheClassNum.isEmpty()) {
        int index = ui->comboBox_4->findText(cacheClassNum);
        if (index >= 0) {
            ui->comboBox_4->setCurrentIndex(index);
        } else {
            ui->comboBox_4->insertItem(0, cacheClassNum);
            ui->comboBox_4->setCurrentIndex(0);
        }
    }

    // 继续级联查询
    QTimer::singleShot(100, this, [this]() {
        if (!ui->comboBox_4->currentText().isEmpty()) {
            ComboBox4to5();
        }
    });
}

void ClassSchedule::onComboBox5DataReceived(const QStringList& data)
{
    QString cacheWeekNum = CacheMap.contains("week_num") ? CacheMap["week_num"].toString() : "";

    ui->comboBox_5->clear();
    ui->comboBox_5->addItems(data);
    ui->comboBox_5->setEnabled(true);

    if (!cacheWeekNum.isEmpty()) {
        int index = ui->comboBox_5->findText(cacheWeekNum);
        if (index >= 0) {
            ui->comboBox_5->setCurrentIndex(index);
        } else {
            ui->comboBox_5->insertItem(0, cacheWeekNum);
            ui->comboBox_5->setCurrentIndex(0);
        }
    }

    // 触发课程表查询
    QTimer::singleShot(100, this, [this]() {
        if (!ui->comboBox_5->currentText().isEmpty()) {
            ComboBox5toG();
        }
    });
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

    // 即使网络失败，也尝试使用缓存数据继续操作
    if (!ui->comboBox_1->currentText().isEmpty() &&
        !ui->comboBox_5->currentText().isEmpty()) {
        qDebug() << "网络失败，尝试使用缓存数据加载课程表";
        QTimer::singleShot(100, this, &ClassSchedule::ComboBox5toG);
    }
}

void ClassSchedule::ComboBox1to2()
{
    QString str_last = ui->comboBox_1->currentText();
    currentCombo2Text = ui->comboBox_2->currentText();
    ui->comboBox_2->clear();

    if (!str_last.isEmpty()) {
        ui->comboBox_2->setEnabled(false);

        // 创建指向当前对象的QPointer
        QPointer<ClassSchedule> safeThis(this);

        dataBase->getClassSchedule(str_last, [safeThis](const QStringList& data) {
            // 回调执行时，先检查对象是否还存在
            if (safeThis) {
                safeThis->onComboBox2DataReceived(data);
            }
            // 如果safeThis为nullptr，则什么都不做，安静地结束回调。
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

        // 创建指向当前对象的QPointer
        QPointer<ClassSchedule> safeThis(this);

        dataBase->getClassSchedule(ui->comboBox_1->currentText(), str_last,
                                   [safeThis](const QStringList& data) {
                                       if (safeThis) {
                                           safeThis->onComboBox3DataReceived(data);
                                       }
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

        // 创建指向当前对象的QPointer
        QPointer<ClassSchedule> safeThis(this);

        dataBase->getClassSchedule(ui->comboBox_1->currentText(),
                                   ui->comboBox_2->currentText(),
                                   str_last,
                                   [safeThis](const QStringList& data) {
                                       if (safeThis) {
                                           safeThis->onComboBox4DataReceived(data);
                                       }
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

        // 创建指向当前对象的QPointer
        QPointer<ClassSchedule> safeThis(this);

        dataBase->getClassSchedule(ui->comboBox_1->currentText(),
                                   ui->comboBox_2->currentText(),
                                   ui->comboBox_3->currentText(),
                                   str_last,
                                   [safeThis](const QStringList& data) {
                                       if (safeThis) {
                                           safeThis->onComboBox5DataReceived(data);
                                       }
                                   });
    } else {
        ui->comboBox_5->setEnabled(false);
    }
}

void ClassSchedule::ComboBox5toG()
{
    // 只有在所有combobox都有有效数据时才触发课程表查询
    if (ui->comboBox_1->currentText().isEmpty() ||
        ui->comboBox_2->currentText().isEmpty() ||
        ui->comboBox_3->currentText().isEmpty() ||
        ui->comboBox_4->currentText().isEmpty() ||
        ui->comboBox_5->currentText().isEmpty()) {
        qDebug() << "部分combobox数据为空，延迟课程表查询";
        QTimer::singleShot(500, this, &ClassSchedule::ComboBox5toG);
        return;
    }

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

    // 创建指向当前对象的QPointer
    QPointer<ClassSchedule> safeThis(this);

    // 异步获取课程表模型
    dataBase->getClassScheduleModel(major, educationLevel, grade, classNum, weekNum,
                                    [safeThis](QPair<bool, bool> result) {
                                        if (safeThis) {
                                            safeThis->onClassScheduleModelReceived(result);
                                        }
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
