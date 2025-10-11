#include "mainwindow.h"
#include"tlogindialog.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    //new一个管理员room。
    QApplication a(argc, argv);
    TloginDialog* loginDialog = new TloginDialog();
    if(loginDialog->exec()==QDialog::Accepted)
    {
        MainWindow w(loginDialog->getSuccessAdmini());
        w.show();
        int result = a.exec();
        delete loginDialog;
        return result;
    }
    delete loginDialog;
}
