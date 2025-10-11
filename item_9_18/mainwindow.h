#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include"relicsroom.h"
class Admini;
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Admini* user, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Admini* m_user;
    relicsRoom* relicsroom;
};
#endif // MAINWINDOW_H
