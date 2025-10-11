#include "curriculum.h"
#include "ui_curriculum.h"
#include"mytablemodel.h"
#include"roundcornerdelegate.h"
#include <QLabel>
#include <QTableView>
#include <QGroupBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include<QHeaderView>
#include<QPainter>
#include <QStyledItemDelegate>
#include <QAbstractTableModel>
#include <QPainterPath>


Curriculum::Curriculum(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Curriculum)
{
    ui->setupUi(this);
    QGroupBox *groupBox1 = new QGroupBox(this);
    QVBoxLayout *groupBox1Layout = new QVBoxLayout(groupBox1);
    QHBoxLayout *hLayout = new QHBoxLayout();

    QComboBox *comboBox1 = new QComboBox(groupBox1);
    comboBox1->addItem("人工智能");
    hLayout->addWidget(comboBox1);

    QComboBox *comboBox2 = new QComboBox(groupBox1);
    comboBox2->addItem("22级");
    hLayout->addWidget(comboBox2);

    QComboBox *comboBox3 = new QComboBox(groupBox1);
    comboBox3->addItem("1班");
    hLayout->addWidget(comboBox3);

    QComboBox *comboBox4 = new QComboBox(groupBox1);
    comboBox4->addItem("11周");
    comboBox4->addItem("10周");
    hLayout->addWidget(comboBox4);


    groupBox1Layout->addLayout(hLayout);

    m_cellInfoLabel = new QLabel("课程： --\n教室： --\n老师： --", groupBox1);
    m_cellInfoLabel->setStyleSheet("background-color: rgba(0, 0, 0, 0.7); color: white; border-radius: 5px; padding: 5px;");
    groupBox1Layout->addWidget(m_cellInfoLabel);

    groupBox1->setLayout(groupBox1Layout);

    QGroupBox *groupBox2 = new QGroupBox(this);
    QVBoxLayout *groupBox2Layout = new QVBoxLayout(groupBox2);
    tableView = new QTableView(groupBox2);
    tableView->setShowGrid(false);  // 隐藏网格线
    tableView->setFrameStyle(QFrame::NoFrame);  // 隐藏表格的边框
    ///////////////////////////////////////////////////////////////////////////////////
    delegate = new RoundCornerDelegate(tableView);
    tableView->setItemDelegate(delegate);
    MyTableModel *model = new MyTableModel();
    ///////////////////////////////////////////////////////////////////////////////////
    tableView->setStyleSheet(QString(
        "QTableView {"
        "   border: 1px solid #ccc;"  // 设置表格边框
        "   border-radius: 10px;"      // 设置整个表格的圆角效果
        "   background-color: #f0f0f0;"  // 设置背景颜色
        "}"
         "QTableView::item:selected {"
         "background-color: #c6e2ff;"               // 设置选中单元格的背景色
         "border: none;"                       // 隐藏选中单元格的边框
         "border-radius: 10px;"                // 设置圆角效果
         "}"
        "QHeaderView::section {"
        "   background-color: #66cc66;"  // 设置表头的背景颜色
        "   border: 1px solid #ccc;"      // 设置表头的边框
        "   border-radius: 5px;"          // 设置表头的圆角效果
        "}"
        ));
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->setModel(model);



    m_cellInfoLabel->setFixedSize(250, 65);
    groupBox2Layout->addWidget(tableView);
    groupBox2->setLayout(groupBox2Layout);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(groupBox1);
    mainLayout->addWidget(groupBox2);

    ui->centralwidget->setLayout(mainLayout);
    setCentralWidget(ui->centralwidget);

    connect(tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &Curriculum::onSelectionChanged);
}

void Curriculum::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected)
    // 获取当前选中的单元格索引
    QModelIndexList selectedIndexes = selected.indexes();
    if (!selectedIndexes.isEmpty())
    {
        QModelIndex index = selectedIndexes.first();  // 获取第一个选中的单元格
        QString cellContent = index.data().toString();  // 获取单元格的内容

        // 在状态栏显示该单元格的内容
        ui->statusBar->showMessage(QString("选择: %1").arg(cellContent));
        m_cellInfoLabel->setText(QString("选择: %1\n选择: \n选择: ").arg(cellContent));
    }
}

Curriculum::~Curriculum()
{
    delete ui;
}




