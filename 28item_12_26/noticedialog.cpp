#include "noticedialog.h"
#include "ui_noticedialog.h"
#include<QListWidget>
#include<QLabel>
#include <QVBoxLayout>
#include<QSplitter>
NoticeDialog::NoticeDialog(QStringList strlist, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NoticeDialog)
{
    ui->setupUi(this);
    setFixedSize(400,600);
    splitter = new QSplitter(Qt::Vertical, this);
    label = new QLabel("公 告");
    QFont font = label->font();
    font.setBold(true);
    font.setPointSize(30);
    label->setFont(font);
    label->setAlignment(Qt::AlignCenter);

    listWidget = new QListWidget();
    listWidget->setFixedSize(400,500);
    for(auto&str:strlist)
    {
        if(str.size()>20)
        {
            int n = str.size()/20;
            for(int i = 1 ;i <= n; ++i)
            {
                str.insert(20 * n + i,'\n');
            }
        }
        listWidget->addItem(" " +  str);
    }
    splitter->addWidget(label);
    splitter->addWidget(listWidget);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(splitter);
    setLayout(layout);
    setWindowTitle("公告");
}

NoticeDialog::~NoticeDialog()
{
    delete ui;
}
