#include "databasemanager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QCollator>
#include <QSqlQueryModel>
#include <QMutex>
#include <QSqlRecord>
#include<QDate>
#include"localcachemanager.h"

// 实现自定义删除器
void DatabaseManager::Deleter::operator()(DatabaseManager* ptr) const
{
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
        connect(m_instance.get(), &DatabaseManager::databaseOpened, m_instance.get(), &DatabaseManager::addPeopleOnline);
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
    if (!connectionName.isEmpty())    QSqlDatabase::removeDatabase(connectionName);
}

bool DatabaseManager::openDatabase()
{
    if (db.isOpen())    return true;
    if (CloudDatabaseManager::openDatabase())
    {
        emit databaseOpened(true);
        return true;
    }
    else
    {
        QString error = db.lastError().text();
        qDebug() << "数据库打开错误:" << error;
        emit databaseOpened(false);
        return false;
    }
}

QStringList DatabaseManager::executeDistinctQuery(const QString &query, const QStringList &params)
{
    QStringList results;
    if (!db.isOpen() && !openDatabase())
    {
        emit queryError("数据库没有打开");
        return results;
    }
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
QDate DatabaseManager::getStartDate()
{
    if (!db.isOpen() && !openDatabase()) {
        emit queryError("数据库未能打开！(起始日期查询)");
        return QDate();
    }

    QString query  = "SELECT start_date FROM start_date WHERE id = 1;";
    QSqlQuery sqlQuery = executeQuery(query);
    if (sqlQuery.lastError().isValid())
    {
        QString error
            = sqlQuery.lastError().text();
        qDebug() << "起始日期查询错误:" << error;
        emit  queryError(error);
        return QDate();
    }

    if (sqlQuery.next()) {
        QString dateString = sqlQuery.value(0).toString();
        QDate date = QDate::fromString(dateString.left(10), "yyyy-MM-dd");
        if (date.isValid())     return date;
        else
        {
            qDebug() << "起始日期格式无效:" << dateString;
            emit  queryError("起始日期格式无效");
            return QDate();
        }
    }

    qDebug() << "未找到起始日期记录";
    emit  queryError("未找到起始日期记录");
    return QDate();
}


QStringList DatabaseManager::getCurrDayClassSchedule()
{
    QDate startDate = getStartDate();
    // 验证起始日期有效性
    if (!startDate.isValid())
    {
        qDebug() << "无效的起始日期";
        return {};
    }
    QDate currentDate = QDate::currentDate();
    int totalDays = startDate.daysTo(currentDate);
    if (totalDays < 0)
    {
        qDebug() << "当前日期在起始日期之前";
        return {};
    }
    int startDayOfWeek = startDate.dayOfWeek();
    int totalOffset = totalDays + startDayOfWeek - 1;
    int curr_week = totalOffset / 7 + 1;
    int curr_day = totalOffset % 7 + 1;
    if (curr_day == 7) curr_day = 0;

    QString query = "SELECT section, course_name, teacher, area, building, room"
                    " FROM class_schedule"
                    " WHERE major = ?"
                    "   AND education_level = ?"
                    "   AND grade = ?"
                    "   AND class_num = ?"
                    "   AND week_num = ?"
                    "   AND week_day = ?;";
    QMap<QString, QVariant> CacheMap = LocalCacheManager::getInstance()->getCachingDataMap();
    QVariantList params =
    {
        CacheMap["major"].toString(),
        CacheMap["education_level"].toString(),
        CacheMap["grade"].toString(),
        CacheMap["class_num"].toString(),
        QString::number(curr_week),
        QString::number(curr_day)
    };
    QStringList fixedResult(5, "");

    QSqlQuery re = executeQuery(query, params);
    if (!re.isActive())
    {
        qDebug() << "查询失败！:" << re.lastError().text();
        return fixedResult; // 返回空列表
    }
    while (re.next())
    {
        // 直接获取字段值（避免字符串分割）
        int section = re.value("section").toInt();
        QString courseName = re.value("course_name").toString();
        QString teacher = re.value("teacher").toString();
        QString area = re.value("area").toString();
        QString building = re.value("building").toString();
        QString room = re.value("room").toString();

        // 只处理0-4的有效节次
        if (section >= 0 && section <= 4) {
            // 组合地点信息
            QStringList locationParts;
            if (!area.isEmpty()) locationParts << area;
            if (!building.isEmpty()) locationParts << building;
            if (!room.isEmpty()) locationParts << room;
            QString location = locationParts.join("-");

            // 组合显示文本
            QString displayText = courseName + " " + teacher;
            if (!location.isEmpty())    displayText += "\n" + location;

            // 将课程放入对应节次位置
            fixedResult[section] = displayText;
        }
    }
    return fixedResult;
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
    while (sqlQuery.next())     results.append(sqlQuery.value(0).toString());
    if (results.isEmpty())     emit queryError("公告列表为空或查询失败");
    return results;
}
