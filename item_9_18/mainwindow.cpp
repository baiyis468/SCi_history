#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<admini.h>
MainWindow::MainWindow(Admini* user, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),m_user(user)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
