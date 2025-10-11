#ifndef NOTICEWIDGET_H
#define NOTICEWIDGET_H

#include <QWidget>

namespace Ui {
class NoticeWidget;
}

class NoticeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NoticeWidget(QWidget *parent = nullptr);
    ~NoticeWidget();

private:
    Ui::NoticeWidget *ui;
signals:
    void noticeQueryFailed();
};

#endif // NOTICEWIDGET_H
