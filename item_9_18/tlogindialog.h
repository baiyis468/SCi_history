#ifndef TLOGINDIALOG_H
#define TLOGINDIALOG_H
class adminiRoom;
#include <QDialog>
class Admini;
namespace Ui {
class TloginDialog;
}

class TloginDialog : public QDialog
{
    Q_OBJECT
public:
    Admini* getSuccessAdmini();
    void getMiniRoom();//暂未实现
public:
    explicit TloginDialog(QWidget *parent = nullptr);
    ~TloginDialog();
private slots:
    void on_btnOK_clicked();

private:
    Admini* adminior;
    int tryCount = 5;
    int tryNow = 0;
    adminiRoom* adminiroom;
    Ui::TloginDialog *ui;
};

#endif // TLOGINDIALOG_H
