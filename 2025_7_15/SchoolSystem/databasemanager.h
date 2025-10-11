#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "clouddatabasemanager.h"  // 包含基类头文件
#include <QStringList>
#include <QSqlQueryModel>
#include <QMutex>

class DatabaseManager : public CloudDatabaseManager {  // 继承自CloudDatabaseManager
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

    Q_INVOKABLE QStringList getNoticeStringList();
    Q_INVOKABLE QSqlQueryModel* getClassScheduleModel(
        const QString &major,
        const QString &educationLevel,
        const QString &grade,
        const QString &classNum,
        int weekNum);

signals:
    void databaseOpened(bool success);
    void queryError(const QString &error);

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    static DatabaseManager* m_instance;
    static QMutex m_mutex;

    QStringList executeDistinctQuery(const QString &query, const QStringList &params = QStringList());
};

#endif // DATABASEMANAGER_H
