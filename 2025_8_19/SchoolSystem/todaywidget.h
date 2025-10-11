#ifndef TODAYWIDGET_H
#define TODAYWIDGET_H

#include <QWidget>

namespace Ui {
class TodayWidget;
}

class TodayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TodayWidget(QWidget *parent = nullptr);
    ~TodayWidget();
private:
    void UpdateTime();
    void UpdateClass();
private slots:
    void GetPeopleOnline();
private:
    Ui::TodayWidget *ui;
    QTimer* timer;
signals:
    void onlineQueryFailed();

};

#endif // TODAYWIDGET_H
