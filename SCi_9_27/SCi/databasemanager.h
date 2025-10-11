#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "clouddatabasemanager.h"
#include <QStringList>
#include <QMutex>
#include <memory>
#include <functional>
#include <QMap>
#include <QRandomGenerator>
#include <QDate>
#include <QTimer>

class DatabaseManager : public CloudDatabaseManager {
    Q_OBJECT
public:
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    static DatabaseManager* instance(QObject* parent = nullptr);

    // 异步查询方法
    void getClassSchedule(std::function<void(const QStringList&)> callback);
    void getClassSchedule(const QString &major, std::function<void(const QStringList&)> callback);
    void getClassSchedule(const QString &major, const QString &educationLevel,std::function<void(const QStringList&)> callback);
    void getClassSchedule(const QString &major, const QString &educationLevel,const QString &grade, std::function<void(const QStringList&)> callback);
    void getClassSchedule(const QString &major, const QString &educationLevel,const QString &grade, const QString &classNum,std::function<void(const QStringList&)> callback);
    void getAreaItems(std::function<void(const QStringList&)> callback);
    void getBuildingItems(const QString &area, std::function<void(const QStringList&)> callback);
    void getEmptyRoom(const QString &area, const QString &building,
                      int week_num, int day_num, std::function<void(const QStringList&)> callback);
    void getOnlineString(std::function<void(const QString&)> callback);
    void getCurrDayClassSchedule(std::function<void(const QStringList&)> callback);
    void getNoticeStringList(std::function<void(const QStringList&)> callback);
    void getClassScheduleModel(const QString &major, const QString &educationLevel,
                               const QString &grade, const QString &classNum,
                               int weekNum, std::function<void(QPair<bool, bool>)> callback);
    void getStartDate(std::function<void(const QDate&)> callback);

    // 在线人数管理
    void addPeopleOnline();
    void prepareForExit();

    // 工具方法
    static QPair<int, int> calculateWeekAndDay(const QDate& startDate);

signals:
    void queryFailed(const QString& error);
    void connectionStatusChanged(bool connected);

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    struct Deleter {
        void operator()(DatabaseManager* ptr) const;
    };
    friend struct Deleter;

    static std::unique_ptr<DatabaseManager, Deleter> m_instance;
    static QMutex m_mutex;

    // 回调管理
    QMap<QString, QPair<QDateTime, std::function<void(const QJsonArray&)>>> m_callbacks;
    QMutex m_callbackMutex;
    QTimer* m_cleanupTimer;

    // 辅助方法
    QString generateRequestId();
    void registerCallback(const QString& requestId, std::function<void(const QJsonArray&)> callback);
    void executeCallback(const QString& requestId, const QJsonArray& results);
    void cleanupExpiredCallbacks();

    // 查询执行方法
    void executeDistinctQuery(const QString& query, const QVariantList& params,
                              std::function<void(const QStringList&)> callback);
    void executeQueryWithRetry(const QString& query, const QVariantList& params,
                               const QString& requestId, int retries = 3);

    // 数据处理方法
    QStringList processEmptyRoomResults(const QJsonArray& results);
    QStringList processCurrentDaySchedule(const QJsonArray& results);
    QVector<QStringList> parseCourseDataFromJson(const QJsonArray& jsonArray);

private slots:
    void onQueryCompleted(const QString &requestId, const QJsonArray &results);
    void onQueryError(const QString &requestId, const QString &error);
    void onConnectionStatusChanged(bool connected);
};

#endif // DATABASEMANAGER_H
