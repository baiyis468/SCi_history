#include "tlogindialog.h"
#include "ui_tlogindialog.h"
#include<QMessageBox>
#include"adminiroom.h"
#include"admini.h"
TLoginDialog::TLoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TLoginDialog)
{
    ui->setupUi(this);
    this->setFixedSize(400,300);
}

TLoginDialog::~TLoginDialog()
{
    delete ui;
}

void TLoginDialog::on_btnOK_clicked()
{
    QString ID = ui->editName->text().trimmed();
    QByteArray m_password = ui->editPSWD->text().toUtf8().trimmed();
    adminiRoom& room = adminiRoom::getInstance();
    Admini* item = dynamic_cast<Admini*>(static_cast<Admini*>(room.findTheData(ID)));
    if(!item||item->getPSWD()!=m_password)
    {
        ++tryNow;
        if(tryNow<tryCount) QMessageBox::critical(this,"错误","用户名或密码错误！");
        else
        {
            QMessageBox::critical(this,"错误","错误次数过多！");
            this->reject();
        }
    }
    else return accept();
}

