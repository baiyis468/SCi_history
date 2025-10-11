#ifndef CLASSSCHEDULE_H
#define CLASSSCHEDULE_H
#include <QWidget>
class QItemDelegate;
class QStandardItemModel;
class DatabaseManager;
class LocalCacheManager;
namespace Ui {class ClassSchedule;}
class ClassSchedule : public QWidget
{
    Q_OBJECT

public:
    explicit ClassSchedule(QWidget *parent = nullptr);
    ~ClassSchedule();
    bool cloudDataSuccess() const { return cloudSuccess; }
    bool cacheDataSuccess() const { return cacheSuccess; }
private slots:
    void ComboBox1to2();
    void ComboBox2to3();
    void ComboBox3to4();
    void ComboBox4to5();
    void ComboBox5toG();
private slots:
    void onCellClicked(const QModelIndex &current, const QModelIndex &previous);//  选中改变label槽函数。
private:
    bool cloudSuccess = true;
    bool cacheSuccess = true;
    Ui::ClassSchedule *ui;
    QItemDelegate* delegate;
    QStandardItemModel* DataModel;
    QMap<QString, QVariant> CacheMap;
    DatabaseManager* dataBase;
    LocalCacheManager* localCache;
};

#endif // CLASSSCHEDULE_H
