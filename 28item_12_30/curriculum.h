    #ifndef CURRICULUM_H
#define CURRICULUM_H
#include <QMainWindow>
class QTableView;
class QItemSelection;
class QLabel;
class RoundCornerDelegate;
class MyTableModel;
class QComboBox;
class getCurriculum;
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
private slots:
    void onComboBox1IndexChanged(const QString &text);
    void onComboBox2IndexChanged(const QString &text);
    void onComboBox3IndexChanged(const QString &text);
    void onComboBox4IndexChanged(const QString &text);
    void onComboBox5IndexChanged(const QString &text);

private:
    QComboBox *comboBox1;
    QComboBox *comboBox2;
    QComboBox *comboBox3;
    QComboBox *comboBox4;
    QComboBox *comboBox5;

private:
    Ui::Curriculum *ui;
    QLabel* m_cellInfoLabel;
    QTableView *tableView;
    RoundCornerDelegate *delegate;
    MyTableModel *model;
    std::map<QString,QMap<QString,QMultiMap<QString,QString>>> map;
};

#endif // CURRICULUM_H
