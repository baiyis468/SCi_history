#ifndef LOADINGWINDOW_H
#define LOADINGWINDOW_H

#include <QWidget>
class QVBoxLayout;
class QLabel;
namespace Ui {
class LoadingWindow;
}

class LoadingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoadingWindow(QWidget *parent = nullptr);
    ~LoadingWindow();

private:
    Ui::LoadingWindow *ui;
    QVBoxLayout* layout;
    QMovie* movie;
};

#endif // LOADINGWINDOW_H
