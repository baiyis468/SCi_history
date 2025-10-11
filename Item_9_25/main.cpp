#include "mainwindow.h"
#include <QApplication>
#include "tlogindialog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TLoginDialog* loginDialog = new TLoginDialog();
    if(loginDialog->exec()==QDialog::Accepted)
    {
        MainWindow w;
        delete loginDialog;
        w.show();
        return 0;
    }
    delete loginDialog;
    return 0;
    //本身都是返回 a.exec()，这里都改成0了。
}
