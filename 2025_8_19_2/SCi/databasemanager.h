#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "clouddatabasemanager.h"
#include <QStringList>
#include <QSqlQueryModel>
#include <QMutex>
#include <memory>
class DatabaseManager : public CloudDatabaseManager {
    Q_OBJECT
public:
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    static DatabaseManager* instance(QObject* parent = nullptr);

    bool openDatabase() override;

    Q_INVOKABLE QStringList getClassSchedule();
    Q_INVOKABLE QStringList getClassSchedule(const QString &major);
    Q_INVOKABLE QStringList getClassSchedule(const QString &major, const QString &educationLevel);
    Q_INVOKABLE QStringList getClassSchedule(const QString &major, const QString &educationLevel, const QString &grade);
    Q_INVOKABLE QStringList getClassSchedule(const QString &major, const QString &educationLevel, const QString &grade, const QString &classNum);
    Q_INVOKABLE QString getOnlineString();

    Q_INVOKABLE QStringList getCurrDayClassSchedule();

    Q_INVOKABLE QStringList getNoticeStringList();
    Q_INVOKABLE QPair<bool,bool> getClassScheduleModel(
        const QString &major,
        const QString &educationLevel,
        const QString &grade,
        const QString &classNum,
        int weekNum);
    void prepareForExit(); // 添加退出准备方法

signals:
    void databaseOpened(bool success);
    void queryError(const QString &error);
private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    struct Deleter {void operator()(DatabaseManager* ptr) const;};
    friend struct Deleter;

    static std::unique_ptr<DatabaseManager, Deleter> m_instance;
    static QMutex m_mutex;
    QStringList executeDistinctQuery(const QString &query, const QStringList &params = QStringList());

private:
    bool opened = false;
    QDate getStartDate();
private slots:
    void addPeopleOnline(bool checked);
};

#endif // DATABASEMANAGER_H
