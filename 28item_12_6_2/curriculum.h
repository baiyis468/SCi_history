#ifndef CURRICULUM_H
#define CURRICULUM_H
#include <QMainWindow>
class QTableView;
class QItemSelection;
class QLabel;
class RoundCornerDelegate;
class MyTableModel;
namespace Ui {
class Curriculum;
}

class Curriculum : public QMainWindow
{
    Q_OBJECT

public:
    explicit Curriculum(QWidget *parent = nullptr);
    ~Curriculum();
private slots:
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
private:
    Ui::Curriculum *ui;
    QLabel* m_cellInfoLabel;
    QTableView *tableView;
    RoundCornerDelegate *delegate;
    MyTableModel *model;
};

#endif // CURRICULUM_H
