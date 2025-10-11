#include "databasemanager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QCollator>
#include <QSqlQueryModel>
#include <QMutex>
#include <QSqlRecord>
// 初始化静态成员
DatabaseManager* DatabaseManager::m_instance = nullptr;
QMutex DatabaseManager::m_mutex;

DatabaseManager* DatabaseManager::instance(QObject* parent)
{
    QMutexLocker locker(&m_mutex);
    if (!m_instance)
    {
        m_instance = new DatabaseManager(parent);
    }
    return m_instance;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : CloudDatabaseManager(parent){}

DatabaseManager::~DatabaseManager() {
    // 关闭数据库连接
    closeDatabase();

    // 移除数据库连接
    QString connectionName = db.connectionName();
    if (!connectionName.isEmpty()) {
        QSqlDatabase::removeDatabase(connectionName);
    }
    m_instance = nullptr;
}

bool DatabaseManager::openDatabase() {
    if (db.isOpen()) {
        return true;
    }
    if (CloudDatabaseManager::openDatabase()) {
        emit databaseOpened(true);
        return true;
    } else {
        QString error = db.lastError().text();
        qDebug() << "数据库打开错误:" << error;
        emit databaseOpened(false);
        return false;
    }
}

QStringList DatabaseManager::executeDistinctQuery(const QString &query, const QStringList &params) {
    QStringList results;
    if (!db.isOpen() && !openDatabase())
    {
        emit queryError("数据库没有打开");
        return results;
    }
    // 将 QStringList 转换为 QVariantList
    QVariantList variantParams;
    for (const QString& param : params)    variantParams.append(param);
    QSqlQuery sqlQuery = executeQuery(query, variantParams);
    if (sqlQuery.lastError().isValid())
    {
        QString error = sqlQuery.lastError().text();
        qDebug() << "查询错误:" << error;
        emit queryError(error);
        return results;
    }
    while (sqlQuery.next())    results.append(sqlQuery.value(0).toString());
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

QStringList DatabaseManager::getClassSchedule(const QString &major)
{
    QString query = "SELECT DISTINCT education_level "
                    "FROM class_schedule "
                    "WHERE major = ? "
                    "ORDER BY education_level ASC";
    return executeDistinctQuery(query, {major});
}

QStringList DatabaseManager::getClassSchedule(const QString &major, const QString &educationLevel)
{
    QString query = "SELECT DISTINCT grade "
                    "FROM class_schedule "
                    "WHERE major = ? AND education_level = ? "
                    "ORDER BY grade ASC";
    return executeDistinctQuery(query, {major, educationLevel});
}

QStringList DatabaseManager::getClassSchedule(const QString &major, const QString &educationLevel, const QString &grade)
{
    QString query = "SELECT DISTINCT class_num "
                    "FROM class_schedule "
                    "WHERE major = ? AND education_level = ? AND grade = ? "
                    "ORDER BY class_num ASC";
    return executeDistinctQuery(query, {major, educationLevel, grade});
}

QStringList DatabaseManager::getClassSchedule(const QString &major, const QString &educationLevel, const QString &grade, const QString &classNum)
{
    QString query = "SELECT DISTINCT week_num "
                    "FROM class_schedule "
                    "WHERE major = ? AND education_level = ? AND grade = ? AND class_num = ? "
                    "ORDER BY week_num ASC";
    return executeDistinctQuery(query, {major, educationLevel, grade, classNum});
}
QStringList DatabaseManager::getNoticeStringList()
{
    QStringList results;
    if (!db.isOpen() && !openDatabase()) {
        emit queryError("数据库没能正常打开！(公告数据库)");
        return results;
    }

    QString query = "SELECT content FROM notices";
    QSqlQuery sqlQuery = executeQuery(query, {});
    if (sqlQuery.lastError().isValid()) {
        QString error = sqlQuery.lastError().text();
        qDebug() << "查询错误:" << error;
        emit queryError(error);
        return results;
    }

    while (sqlQuery.next()) {
        results.append(sqlQuery.value(0).toString());
    }

    return results;
}
QSqlQueryModel *DatabaseManager::getClassScheduleModel(
    const QString &major,
    const QString &educationLevel,
    const QString &grade,
    const QString &classNum,
    int weekNum)
{

    QSqlQueryModel *model = new QSqlQueryModel(this);
    if (!db.isOpen() && !openDatabase()) {
        emit queryError("数据库没能正常打开！(课程表数据库)");
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
    QSqlQuery sqlQuery = executeQuery(query,{major,educationLevel,grade,classNum,weekNum});
    if (sqlQuery.lastError().isValid())
    {
        QString error = sqlQuery.lastError().text();
        qDebug() << "查询错误:" << error;
        emit queryError(error);
        return model;
    }
    model->setQuery(std::move(sqlQuery));
    return model;
}
