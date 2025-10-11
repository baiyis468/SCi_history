#ifndef FINDWIDGET_H
#define FINDWIDGET_H

#include <QWidget>

namespace Ui {
class FindWidget;
}

class FindWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FindWidget(QWidget *parent = nullptr);
    ~FindWidget();

private slots:
    void on_pushButton_clicked();

    void on_areaComboBox_currentTextChanged(const QString &arg1);

private:
    Ui::FindWidget *ui;
};

#endif // FINDWIDGET_H
