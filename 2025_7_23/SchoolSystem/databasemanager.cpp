#include "databasemanager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QCollator>
#include <QSqlQueryModel>
#include <QMutex>
#include <QSqlRecord>

// 实现自定义删除器
void DatabaseManager::Deleter::operator()(DatabaseManager* ptr) const {
    delete ptr;
}

// 初始化 unique_ptr
std::unique_ptr<DatabaseManager, DatabaseManager::Deleter> DatabaseManager::m_instance = nullptr;
QMutex DatabaseManager::m_mutex;

DatabaseManager* DatabaseManager::instance(QObject* parent)
{
    QMutexLocker locker(&m_mutex);
    if (!m_instance)
    {
        m_instance = std::unique_ptr<DatabaseManager, Deleter>(new DatabaseManager(parent));
        connect(m_instance.get(), &DatabaseManager::databaseOpened,
                m_instance.get(), &DatabaseManager::addPeopleOnline);
    }
    return m_instance.get();
}

DatabaseManager::DatabaseManager(QObject *parent)
    : CloudDatabaseManager(parent){}

DatabaseManager::~DatabaseManager()
{
    if(opened) this->executeUpdate("UPDATE number_people_online SET val = val - 1 WHERE id = 1;");
    // 关闭数据库连接
    closeDatabase();

    // 移除数据库连接
    QString connectionName = db.connectionName();
    if (!connectionName.isEmpty()) {
        QSqlDatabase::removeDatabase(connectionName);
    }
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

void DatabaseManager::addPeopleOnline(bool checked)
{
    if(!opened)
    {
        opened = true;
        this->executeUpdate("UPDATE number_people_online SET val = val + 1 WHERE id = 1;");
    }
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
// databasemanager.cpp
QString DatabaseManager::getOnlineString()
{
    if (!db.isOpen() && !openDatabase()) {
        emit queryError("数据库未能打开！(在线人数查询)");
        return "0"; // 返回字符串形式的默认值
    }

    QString query = "SELECT val FROM number_people_online WHERE id = 1;";
    QSqlQuery sqlQuery = executeQuery(query);

    if (sqlQuery.lastError().isValid()) {
        QString error = sqlQuery.lastError().text();
        qDebug() << "在线人数查询错误:" << error;
        emit queryError(error);
        return "0";
    }

    if (sqlQuery.next()) {
        return QString::number(sqlQuery.value(0).toUInt());
    }

    qDebug() << "未找到在线人数记录";
    emit queryError("未找到在线人数记录");
    return "0";
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

    if (results.isEmpty()) {
        emit queryError("公告列表为空或查询失败");
    }

    return results;
}
