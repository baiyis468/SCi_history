#ifndef LOCALCACHEMANAGER_H
#define LOCALCACHEMANAGER_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <QSqlQueryModel>
#include <QMap>
#include <QVector>
class QStandardItemModel;
class LocalCacheManager {
public:
    static LocalCacheManager* getInstance();
    ~LocalCacheManager();
    bool openDatabase(const QString& dbName = "local_cache.db");
    void closeDatabase();

    // 缓存表操作
    bool updateCachingData(const QString& major, const QString& educationLevel, const QString& grade, const QString& classNum, int weekNum, bool updated);
    QMap<QString, QVariant> getCachingDataMap();

    // 课程表操作
    bool replaceCoursesTable(QSqlQueryModel* model);
    QStandardItemModel* getCoursesTableModel();

private:
    LocalCacheManager();
    LocalCacheManager(const LocalCacheManager&) = delete;
    LocalCacheManager& operator=(const LocalCacheManager&) = delete;

    static LocalCacheManager* instance;
    QSqlDatabase db;

    bool createTables();
    bool saveCachingData(const QString& major, const QString& educationLevel, const QString& grade, const QString& classNum, int weekNum, bool updated);
    bool clearCachingTable();
    bool clearCoursesTable();
    bool insertCourse(int weekDay, int section, const QString& courseName, const QString& teacher,const QString& area, const QString& building, const QString& room);

    QColor getContrastTextColor(const QColor &bgColor);
};

#endif // LOCALCACHEMANAGER_H
