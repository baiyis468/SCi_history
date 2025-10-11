#include "databasemanager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QCollator>
#include <QSqlQueryModel>
#include <QMutex>
#include <QSqlRecord>

// 初始化静态成员
DatabaseManager* DatabaseManager::m_instance = nullptr;
QMutex DatabaseManager::m_mutex;

DatabaseManager* DatabaseManager::instance(QObject* parent) {
    QMutexLocker locker(&m_mutex);
    if (!m_instance)
    {
        m_instance = new DatabaseManager(parent);
    }
    return m_instance;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent) {

    db = QSqlDatabase::addDatabase("QMYSQL", "singleton_connection");
    db.setHostName("mysql2.sqlpub.com");
    db.setDatabaseName("xiaoyuanapp");
    db.setUserName("xiaoyuanapp");
    //db.setPassword("");
    db.setPort(3307);
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
    // 移除数据库连接
    QSqlDatabase::removeDatabase("singleton_connection");
    m_instance = nullptr;
}

bool DatabaseManager::openDatabase() {
    if (db.isOpen()) {
        return true;
    }

    if (db.open()) {
        emit databaseOpened(true);
        return true;
    } else {
        QString error = db.lastError().text();
        qDebug() << "Database open error:" << error;
        emit databaseOpened(false);
        return false;
    }
}



QStringList DatabaseManager::executeDistinctQuery(const QString &query, const QStringList &params) {
    QStringList results;

    if (!db.isOpen() && !openDatabase()) {
        emit queryError("Database is not open");
        return results;
    }

    QSqlQuery sqlQuery(db);
    sqlQuery.prepare(query);

    // 绑定参数
    for (int i = 0; i < params.size(); ++i) {
        sqlQuery.bindValue(i, params[i]);
    }

    if (!sqlQuery.exec()) {
        QString error = sqlQuery.lastError().text();
        qDebug() << "Query error:" << error;
        emit queryError(error);
        return results;
    }

    while (sqlQuery.next()) {
        results.append(sqlQuery.value(0).toString());
    }

    // 确保字典序排序
    QCollator collator;
    collator.setNumericMode(true); // 启用数字识别模式
    std::sort(results.begin(), results.end(), collator);

    return results;
}
QStringList DatabaseManager::getClassSchedule()
{
    QString query = "SELECT DISTINCT major "
                    "FROM class_schedule "
                    "ORDER BY major ASC";
    return executeDistinctQuery(query);
}
// 1. 查询指定major下的所有education_level（按字典序排序）
QStringList DatabaseManager::getClassSchedule(const QString &major)
{
    QString query = "SELECT DISTINCT education_level "
                    "FROM class_schedule "
                    "WHERE major = ? "
                    "ORDER BY education_level ASC";
    return executeDistinctQuery(query, {major});
}
// 2. 查询指定major和education_level下的所有grade（按字典序排序）
QStringList DatabaseManager::getClassSchedule(const QString &major, const QString &educationLevel)
{
    QString query = "SELECT DISTINCT grade "
                    "FROM class_schedule "
                    "WHERE major = ? AND education_level = ? "
                    "ORDER BY grade ASC";
    return executeDistinctQuery(query, {major, educationLevel});
}
// 3. 查询指定major、education_level和grade下的所有class_num（按字典序排序）
QStringList DatabaseManager::getClassSchedule(const QString &major, const QString &educationLevel, const QString &grade) {
    QString query = "SELECT DISTINCT class_num "
                    "FROM class_schedule "
                    "WHERE major = ? AND education_level = ? AND grade = ? "
                    "ORDER BY class_num ASC";
    return executeDistinctQuery(query, {major, educationLevel, grade});
}
// 4. 查询指定major、education_level、grade和class_num下的所有week_num（按字典序排序）
QStringList DatabaseManager::getClassSchedule(const QString &major, const QString &educationLevel, const QString &grade, const QString &classNum)
{
    QString query = "SELECT DISTINCT week_num "
                    "FROM class_schedule "
                    "WHERE major = ? AND education_level = ? AND grade = ? AND class_num = ? "
                    "ORDER BY week_num ASC";
    return executeDistinctQuery(query, {major, educationLevel, grade, classNum});
}
QSqlQueryModel *DatabaseManager::getClassScheduleModel(
    const QString &major,
    const QString &educationLevel,
    const QString &grade,
    const QString &classNum,
    int weekNum) {

    auto *model = new QSqlQueryModel(this);
    if (!db.isOpen() && !openDatabase()) {
        emit queryError("Database is not open");
        return model;
    }

    // 准备SQL查询
    QString query =
        "SELECT week_day, section, course_name, teacher, area, building, room "
        "FROM class_schedule "
        "WHERE major = ? "
        "  AND education_level = ? "
        "  AND grade = ? "
        "  AND class_num = ? "
        "  AND week_num = ? "
        "ORDER BY week_day, section";

    QSqlQuery sqlQuery(db);
    sqlQuery.prepare(query);

    // 绑定参数
    sqlQuery.bindValue(0, major);
    sqlQuery.bindValue(1, educationLevel);
    sqlQuery.bindValue(2, grade);
    sqlQuery.bindValue(3, classNum);
    sqlQuery.bindValue(4, weekNum);

    if (!sqlQuery.exec()) {
        QString error = sqlQuery.lastError().text();
        qDebug() << "Query error:" << error;
        emit queryError(error);
        return model;
    }
    model->setQuery(std::move(sqlQuery));
    return model;
}
