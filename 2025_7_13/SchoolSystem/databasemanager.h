#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QStringList>
#include <QSqlQueryModel>
#include <QMutex>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    static DatabaseManager* instance(QObject* parent = nullptr);

    Q_INVOKABLE bool openDatabase();

    // 重载查询函数
    Q_INVOKABLE QStringList getClassSchedule();
    Q_INVOKABLE QStringList getClassSchedule(const QString &major);
    Q_INVOKABLE QStringList getClassSchedule(const QString &major, const QString &educationLevel);
    Q_INVOKABLE QStringList getClassSchedule(const QString &major, const QString &educationLevel, const QString &grade);
    Q_INVOKABLE QStringList getClassSchedule(const QString &major, const QString &educationLevel, const QString &grade, const QString &classNum);

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
    // 私有构造函数
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    // 单例实例指针
    static DatabaseManager* m_instance;
    static QMutex m_mutex; // 线程安全锁

    QSqlDatabase db;

    // 确保字典序
    QStringList executeDistinctQuery(const QString &query, const QStringList &params = QStringList());
};

#endif // DATABASEMANAGER_H
