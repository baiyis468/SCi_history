#include "mainwindow.h"
#include <QApplication>
#include "tlogindialog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TLoginDialog* loginDialog = new TLoginDialog();
    if(loginDialog->exec()==QDialog::Accepted)
    {
        //MainWindow w(loginDialog->getSuccessAdmini());
        delete loginDialog;
        //w.show();
        int result = a.exec();
        return result;
    }
    delete loginDialog;
    return a.exec();
}
