#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>

namespace Ui {
class SettingWidget;
}

class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWidget(QWidget *parent = nullptr);
    ~SettingWidget();

private slots:

    void on_ApplyButton_clicked();

private:
    Ui::SettingWidget *ui;
};

#endif // SETTINGWIDGET_H
