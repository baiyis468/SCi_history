#ifndef CLASSSCHEDULE_H
#define CLASSSCHEDULE_H

#include <QWidget>
#include <QMap>
#include <QVariant>

class QItemDelegate;
class QStandardItemModel;
class DatabaseManager;
class LocalCacheManager;

namespace Ui {
class ClassSchedule;
}

class ClassSchedule : public QWidget
{
    Q_OBJECT

public:
    explicit ClassSchedule(QWidget *parent = nullptr);
    ~ClassSchedule();

    bool cloudDataSuccess() const { return cloudSuccess; }
    bool cacheDataSuccess() const { return cacheSuccess; }

private slots:
    void onComboBox1DataReceived(const QStringList& data);
    void onComboBox2DataReceived(const QStringList& data);
    void onComboBox3DataReceived(const QStringList& data);
    void onComboBox4DataReceived(const QStringList& data);
    void onComboBox5DataReceived(const QStringList& data);
    void onClassScheduleModelReceived(QPair<bool, bool> result);

    void ComboBox1to2();
    void ComboBox2to3();
    void ComboBox3to4();
    void ComboBox4to5();
    void ComboBox5toG();

    void onCellClicked(const QModelIndex &current, const QModelIndex &previous);
    void onQueryError(const QString& error);

private:
    bool cloudSuccess = true;
    bool cacheSuccess = true;
    Ui::ClassSchedule *ui;
    QItemDelegate* delegate;
    QStandardItemModel* DataModel;
    QMap<QString, QVariant> CacheMap;
    DatabaseManager* dataBase;
    LocalCacheManager* localCache;

    // 用于跟踪当前请求的ID
    QString currentRequestId;

    // 保存当前选择的值，用于异步回调
    QString currentCombo1Text;
    QString currentCombo2Text;
    QString currentCombo3Text;
    QString currentCombo4Text;
    QString currentCombo5Text;
};

#endif // CLASSSCHEDULE_H
