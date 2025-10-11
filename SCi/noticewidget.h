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

private slots:
    void onNoticesReceived(const QStringList& notices);
    void onQueryError(const QString& error);

private:
    Ui::NoticeWidget *ui;

signals:
    void noticeQueryFailed();
};

#endif // NOTICEWIDGET_H
