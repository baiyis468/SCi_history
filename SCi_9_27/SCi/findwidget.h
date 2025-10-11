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
    void onAreaItemsReceived(const QStringList& areas);
    void onBuildingItemsReceived(const QStringList& buildings);
    void onEmptyRoomResultsReceived(const QStringList& rooms);
    void onQueryError(const QString& error);

    void on_pushButton_clicked();
    void on_areaComboBox_currentTextChanged(const QString &arg1);

private:
    Ui::FindWidget *ui;
};

#endif // FINDWIDGET_H
