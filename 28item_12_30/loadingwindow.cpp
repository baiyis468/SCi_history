#include "loadingwindow.h"
#include "ui_loadingwindow.h"
#include<QVBoxLayout>
#include<QLabel>
#include<QMovie>
#include<QTimer>
LoadingWindow::LoadingWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoadingWindow)
{
    ui->setupUi(this);
    setFixedSize(400,600);
    setWindowTitle("加载动画");
    layout = new QVBoxLayout(this);
    QLabel* label =  new QLabel(this);
    movie = new QMovie(":/new/prefix1/C:/Users/86135/Desktop/loading.gif");
    label->setMovie(movie);
    movie->start();
    layout->addWidget(label);

    setLayout(layout);
    QTimer::singleShot(1000, this,[this,label]() {
        label->hide();
        movie->stop();
        this->close();
    });
}

LoadingWindow::~LoadingWindow()
{
    delete ui;
}
