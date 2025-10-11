#ifndef NOTICEDIALOG_H
#define NOTICEDIALOG_H
#include <QDialog>
class QListWidget;
class QSplitter;
class QLabel;
namespace Ui {
class NoticeDialog;
}

class NoticeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoticeDialog(QStringList strlist,QWidget *parent = nullptr);
    ~NoticeDialog();

private:
    Ui::NoticeDialog *ui;
    QListWidget* listWidget;
    QSplitter* splitter;
    QLabel* label;
};

#endif // NOTICEDIALOG_H
