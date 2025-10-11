#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "classschedule.h"

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

    // 页面 - 只保留已实现的ClassSchedule
    ClassSchedule* classScheduleWidget = nullptr;
    // 其他页面暂时不需要声明
};

#endif // MAINWINDOW_H
