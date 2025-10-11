#include "curriculum.h"
#include "ui_curriculum.h"
#include"mytablemodel.h"
#include"roundcornerdelegate.h"
#include"getcurriculum.h"
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
#include<QPair>
Curriculum::Curriculum(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Curriculum)
{
    ui->setupUi(this);
    QGroupBox *groupBox1 = new QGroupBox(this);
    QVBoxLayout *groupBox1Layout = new QVBoxLayout(groupBox1);
    QHBoxLayout *hLayout = new QHBoxLayout();
    comboBox1 = new QComboBox(groupBox1);
    getCurriculum& curriculum = getCurriculum::getInstance();
    ///////////////////
    curriculum.getInstance().getCloudClass("sss");
    /////////////////////
    QMap<QString, QMap<QString, QMultiMap<QString, QString>>> curriculumMap = curriculum.getMap();



    map = curriculumMap.toStdMap();
    for ( auto& [x,y] : map)   comboBox1->addItem(x);
    hLayout->addWidget(comboBox1);

    comboBox2 = new QComboBox(groupBox1);
    comboBox2->setEnabled(false);
    hLayout->addWidget(comboBox2);

    comboBox3 = new QComboBox(groupBox1);
    comboBox3->setEnabled(false);
    hLayout->addWidget(comboBox3);

    comboBox4 = new QComboBox(groupBox1);
    comboBox4->setEnabled(false);
    hLayout->addWidget(comboBox4);

    comboBox5 = new QComboBox(groupBox1);
    for(int i =1;i<20;++i)
    {
        comboBox5->addItem(QString::number(i)+"周");
    }
    hLayout->addWidget(comboBox5);

    groupBox1Layout->addLayout(hLayout);

    m_cellInfoLabel = new QLabel("课程： --\n教室： --\n老师： --", groupBox1);
    m_cellInfoLabel->setStyleSheet("background-color: rgba(0, 0, 0, 0.7); color: white; border-radius: 5px; padding: 5px;");
    groupBox1Layout->addWidget(m_cellInfoLabel);

    groupBox1->setLayout(groupBox1Layout);

    QGroupBox *groupBox2 = new QGroupBox(this);
    QVBoxLayout *groupBox2Layout = new QVBoxLayout(groupBox2);

    tableView = new QTableView(groupBox2);



    MyTableModel *model = new MyTableModel();
    tableView->setModel(model);
    for (int i = 0; i < model->columnCount(); ++i)
    {
        tableView->horizontalHeader()->resizeSection(i, 60);
    }

    for (int i = 0; i < model->rowCount(); ++i) {
        tableView->verticalHeader()->resizeSection(i, 125);
    }
    tableView->setShowGrid(false);  // 隐藏网格线
    tableView->setFrameStyle(QFrame::NoFrame);  // 隐藏表格的边框

    delegate = new RoundCornerDelegate(tableView);
    tableView->setItemDelegate(delegate);

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

    connect(comboBox1, &QComboBox::textActivated, this, &Curriculum::onComboBox1IndexChanged);
    connect(comboBox2, &QComboBox::textActivated, this, &Curriculum::onComboBox2IndexChanged);
    connect(comboBox3, &QComboBox::textActivated, this, &Curriculum::onComboBox3IndexChanged);
    connect(comboBox4, &QComboBox::textActivated, this, &Curriculum::onComboBox4IndexChanged);

    curriculum.getInstance().getMap();
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
void Curriculum::onComboBox1IndexChanged(const QString &text)
{
    auto m = map[text].toStdMap();
    comboBox2->clear();
    comboBox3->clear();
    comboBox4->clear();

    for ( auto& [x,y] : m)
    {
        comboBox2->addItem(x);//加入层次
    }

    comboBox2->setEnabled(true);
    comboBox3->setEnabled(false);
    comboBox4->setEnabled(false);
    if(comboBox2->count()==1)   onComboBox2IndexChanged(comboBox2->currentText());

}

void Curriculum::onComboBox2IndexChanged(const QString &text)
{
    comboBox3->clear();
    comboBox4->clear();
    auto m = map[comboBox1->currentText()][text].toStdMultiMap();
    for(auto&[x,y]:m)
    {
        if(comboBox3->findText(x)== -1) comboBox3->addItem(x);
    }
    comboBox2->setEnabled(true);
    comboBox3->setEnabled(true);
    comboBox4->setEnabled(false);
    if(comboBox3->count()==1)   onComboBox3IndexChanged(comboBox3->currentText());
}

void Curriculum::onComboBox3IndexChanged(const QString &text)
{
    comboBox4->clear();
    auto m = map[comboBox1->currentText()][comboBox2->currentText()];
    QList<QString> list = m.values(text);
    for(auto&x:list)
    {
        comboBox4->addItem(x);
    }
    comboBox2->setEnabled(true);
    comboBox3->setEnabled(true);
    comboBox4->setEnabled(true);
}

void Curriculum::onComboBox4IndexChanged(const QString &text)
{
    QString FileName ="_" + comboBox1->currentText() + "_" + comboBox2->currentText() +
                       "_" + comboBox3->currentText() + "_" + text+".txt";
}

void Curriculum::onComboBox5IndexChanged(const QString &text)
{
    if(comboBox4->isEnabled())
    {
        //如果班级框已经被启用了。。
    }
    else return;
}
Curriculum::~Curriculum()
{
    delete ui;
}




