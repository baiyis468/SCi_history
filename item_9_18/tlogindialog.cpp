#include "tlogindialog.h"
#include "ui_tlogindialog.h"
#include"adminiroom.h"
#include<QMessageBox>
Admini* TloginDialog::getSuccessAdmini()
{
    return adminior;
}
TloginDialog::TloginDialog(QWidget *parent)
    : QDialog(parent)
    ,adminiroom(new adminiRoom)
    , ui(new Ui::TloginDialog)
{
    ui->setupUi(this);
}

TloginDialog::~TloginDialog()
{
    delete ui;
    delete adminiroom;
}

void TloginDialog::on_btnOK_clicked()
{
    QString ID = ui->editName->text().trimmed();
    QByteArray m_password = ui->Editpassword->text().toUtf8().trimmed();
    adminior = adminiroom->FindAdmini(ID,m_password);
    if(adminior)  return accept();
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

