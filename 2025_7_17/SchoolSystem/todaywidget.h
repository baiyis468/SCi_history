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
private slots:
    void UpdateTime();
    void GetPeopleOnline();
private:
    Ui::TodayWidget *ui;
    QTimer* timer;
    QTimer* timer_2;
};

#endif // TODAYWIDGET_H
