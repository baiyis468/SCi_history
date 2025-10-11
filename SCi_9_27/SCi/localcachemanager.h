#ifndef LOCALCACHEMANAGER_H
#define LOCALCACHEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QVector>
#include <memory>

class QStandardItemModel;
class LocalCacheManager {
public:
    static LocalCacheManager* getInstance();
    ~LocalCacheManager();

    bool openDatabase(const QString& dbName = "local_cache.db");
    void closeDatabase();

    // 检查是不是初始化了？
    bool isOpen() const;

    // 缓存表操作
    bool updateCachingData(const QString& major, const QString& educationLevel,
                           const QString& grade, const QString& classNum,
                           int weekNum, bool updated);
    QMap<QString, QVariant> getCachingDataMap();

    // 课程表操作 - 新版本使用 QVector<QStringList>
    bool storeCourseData(const QVector<QStringList>& courseData);
    QStandardItemModel* getCoursesTableModel();

    // 偏差日期操作
    bool updateToDays(int newValue);
    int getToDays();

private:
    LocalCacheManager();

    // 自定义删除器声明
    struct Deleter {
        void operator()(LocalCacheManager* ptr) const;
    };

    friend struct Deleter; // 友元关系允许访问私有析构函数

    LocalCacheManager(const LocalCacheManager&) = delete;
    LocalCacheManager& operator=(const LocalCacheManager&) = delete;

    static std::unique_ptr<LocalCacheManager, Deleter> instance;
    QSqlDatabase db;

    bool createTables();
    bool saveCachingData(const QString& major, const QString& educationLevel,
                         const QString& grade, const QString& classNum,
                         int weekNum, bool updated);
    bool clearCachingTable();
    bool clearCoursesTable();
    bool insertCourse(int weekDay, int section, const QString& courseName,
                      const QString& teacher, const QString& area,
                      const QString& building, const QString& room);

    bool initialized = false; // 初始化标志
};

#endif // LOCALCACHEMANAGER_H
