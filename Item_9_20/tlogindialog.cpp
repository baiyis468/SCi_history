#include "tlogindialog.h"
#include "ui_tlogindialog.h"
#include<QMessageBox>
#include"adminiroom.h"
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
    if(true)  return accept();    //这里还没做好。
    else
    {
        ++tryNow;
        if(tryNow<tryCount) QMessageBox::critical(this,"错误","用户名或密码错误！");
        else
        {
            QMessageBox::critical(this,"错误","错误次数过多！");
            this->reject();
        }
    }
}

