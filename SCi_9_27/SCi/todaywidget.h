#ifndef TODAYWIDGET_H
#define TODAYWIDGET_H

#include <QWidget>
#include <QTimer>

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
    void onOnlineCountReceived(const QString& onlineCount);
    void onClassScheduleReceived(const QStringList& classSchedule);
    void onQueryError(const QString& error);

private:
    Ui::TodayWidget *ui;
    QTimer* timer;

signals:
    void onlineQueryFailed();
};

#endif // TODAYWIDGET_H
