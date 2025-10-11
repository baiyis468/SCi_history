#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "classschedule.h"
#include "noticewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // 页面
    ClassSchedule* classScheduleWidget = nullptr;
    NoticeWidget* noticeWidget = nullptr;

    // 辅助函数：创建占位页
    QWidget* createPlaceholderPage(const QString& message);
};

#endif // MAINWINDOW_H
