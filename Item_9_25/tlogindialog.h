#ifndef TLOGINDIALOG_H
#define TLOGINDIALOG_H

#include <QDialog>

namespace Ui {
class TLoginDialog;
}

class TLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TLoginDialog(QWidget *parent = nullptr);
    ~TLoginDialog();
private slots:
    void on_btnOK_clicked();

private:
    int tryCount = 5;
    int tryNow = 0;
    Ui::TLoginDialog *ui;
};

#endif // TLOGINDIALOG_H
