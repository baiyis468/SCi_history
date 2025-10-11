#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form.h"
#include "curriculum.h"
#include"noticelabel.h"
#include"loadingwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    LoadingWindow* loadingWidget = new LoadingWindow(this);
    loadingWidget->show();
    setFixedSize(400,600);
    QStringList strlist;strlist
                        <<"新公告: 欢迎使用新的公告系统！   "
                        <<"   注意: 系统将于今晚进行维护！   "
                        <<" 特别活动: 本周六有个折扣活动！乌萨奇超市满199元赠送十二折优惠券！   "
                        <<" 震惊！一男子深夜竟在半夜上九楼发现安放在九楼的C4炸弹！背后原因竟然是..."
                        <<"  特别新闻：蔡徐坤今日闻鸡起舞";
    NoticeLabel* noticelabel = new NoticeLabel(strlist);
    ui->statusBar->addWidget(noticelabel);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    Form* f = new Form();
    f->show();
}


void MainWindow::on_pushButton_2_clicked()
{
    Curriculum* f = new Curriculum();
    f->show();
}

