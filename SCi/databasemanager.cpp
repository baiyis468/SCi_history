#include "databasemanager.h"
#include "localcachemanager.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QCollator>
#include <QNetworkReply>
#include <QRandomGenerator>
#include <QDateTime>

// 初始化静态成员
std::unique_ptr<DatabaseManager, DatabaseManager::Deleter> DatabaseManager::m_instance = nullptr;
QMutex DatabaseManager::m_mutex;

void DatabaseManager::Deleter::operator()(DatabaseManager* ptr) const
{
    delete ptr;
}

DatabaseManager* DatabaseManager::instance(QObject* parent)
{
    QMutexLocker locker(&m_mutex);
    if (!m_instance)
    {
        m_instance = std::unique_ptr<DatabaseManager, Deleter>(new DatabaseManager(parent));
    }
    return m_instance.get();
}

DatabaseManager::DatabaseManager(QObject *parent)
    : CloudDatabaseManager(parent)
{
    // 连接信号
    connect(this, &CloudDatabaseManager::queryCompleted,
            this, &DatabaseManager::onQueryCompleted);
    connect(this, &CloudDatabaseManager::queryError,
            this, &DatabaseManager::onQueryError);
    connect(this, &CloudDatabaseManager::connectionStatusChanged,
            this, &DatabaseManager::onConnectionStatusChanged);

    // 设置回调清理定时器
    m_cleanupTimer = new QTimer(this);
    connect(m_cleanupTimer, &QTimer::timeout, this, &DatabaseManager::cleanupExpiredCallbacks);
    m_cleanupTimer->start(30000); // 每30秒清理一次
}

DatabaseManager::~DatabaseManager()
{
    if (m_cleanupTimer) {
        m_cleanupTimer->stop();
        delete m_cleanupTimer;
    }

    // 清理所有未完成的回调
    QMutexLocker locker(&m_callbackMutex);
    m_callbacks.clear();
}

QString DatabaseManager::generateRequestId()
{
    return QString::number(QDateTime::currentMSecsSinceEpoch()) +
           QString::number(QRandomGenerator::global()->generate());
}

void DatabaseManager::registerCallback(const QString& requestId, std::function<void(const QJsonArray&)> callback)
{
    QMutexLocker locker(&m_callbackMutex);
    m_callbacks[requestId] = qMakePair(QDateTime::currentDateTime(), callback);
}

void DatabaseManager::executeCallback(const QString& requestId, const QJsonArray& results)
{
    QMutexLocker locker(&m_callbackMutex);
    if (m_callbacks.contains(requestId)) {
        auto callbackPair = m_callbacks.take(requestId);
        callbackPair.second(results);
    }
}

void DatabaseManager::cleanupExpiredCallbacks()
{
    QMutexLocker locker(&m_callbackMutex);
    QDateTime now = QDateTime::currentDateTime();
    auto it = m_callbacks.begin();
    while (it != m_callbacks.end()) {
        if (it.value().first.secsTo(now) > 30) { // 30秒超时
            qDebug() << "Request timeout:" << it.key();
            it = m_callbacks.erase(it);
        } else {
            ++it;
        }
    }
}

void DatabaseManager::executeDistinctQuery(const QString& query, const QVariantList& params,
                                           std::function<void(const QStringList&)> callback)
{
    QString requestId = generateRequestId();

    // 从查询中提取列名
    QString columnName = "value";
    QRegularExpression re("SELECT\\s+DISTINCT\\s+([^\\s]+)");
    QRegularExpressionMatch match = re.match(query);
    if (match.hasMatch()) {
        columnName = match.captured(1);
    }

    // 注册回调
    registerCallback(requestId, [callback, columnName](const QJsonArray& results) {
        QStringList list;
        for (const auto& value : results) {
            if (value.isObject()) {
                QJsonObject obj = value.toObject();
                if (obj.contains(columnName)) {
                    list.append(obj[columnName].toString());
                } else if (!obj.isEmpty()) {
                    list.append(obj.begin().value().toString());
                }
            } else if (value.isString()) {
                list.append(value.toString());
            }
        }

        // 确保字典序排序
        QCollator collator;
        collator.setNumericMode(true);
        std::sort(list.begin(), list.end(), collator);

        callback(list);
    });

    // 执行查询
    executeQueryWithRetry(query, params, requestId);
}

void DatabaseManager::executeQueryWithRetry(const QString& query, const QVariantList& params,
                                            const QString& requestId, int retries)
{
    if (retries <= 0) {
        emit queryError(requestId, "Maximum retries exceeded");
        return;
    }

    // 执行查询
    CloudDatabaseManager::executeQuery(query, params, requestId);

    // 设置超时重试
    QTimer::singleShot(5000, [=]() {
        QMutexLocker locker(&m_callbackMutex);
        if (m_callbacks.contains(requestId)) {
            qDebug() << "Retrying query:" << requestId;
            executeQueryWithRetry(query, params, requestId, retries - 1);
        }
    });
}

void DatabaseManager::onQueryCompleted(const QString &requestId, const QJsonArray &results)
{
    executeCallback(requestId, results);
}

void DatabaseManager::onQueryError(const QString &requestId, const QString &error)
{
    QMutexLocker locker(&m_callbackMutex);
    if (m_callbacks.contains(requestId)) {
        m_callbacks.remove(requestId);
        emit queryFailed(error);
    }
}

void DatabaseManager::onConnectionStatusChanged(bool connected)
{
    emit connectionStatusChanged(connected);
}

// 数据处理方法
QStringList DatabaseManager::processEmptyRoomResults(const QJsonArray& results)
{
    QStringList list;
    QMap<QString, QStringList> roomSections;

    QMap<int, QString> sectionMap = {
        {0, "第一节无课"},
        {1, "第二节无课"},
        {2, "第三节无课"},
        {3, "第四节无课"},
        {4, "晚自习无课"}
    };

    for (const auto& value : results) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            QString room = obj["room"].toString();
            int section = obj["section"].toInt();

            if (sectionMap.contains(section)) {
                if (!roomSections.contains(room)) {
                    roomSections[room] = QStringList();
                }
                roomSections[room].append(sectionMap[section]);
            }
        }
    }

    for (auto it = roomSections.begin(); it != roomSections.end(); ++it) {
        QString room = it.key();
        QStringList sections = it.value();
        list.append(room + " " + sections.join(" "));
    }

    list.sort();
    return list;
}

QStringList DatabaseManager::processCurrentDaySchedule(const QJsonArray& results)
{
    QStringList fixedResult(5, "");

    for (const auto& value : results) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            int section = obj["section"].toInt();
            QString courseName = obj["course_name"].toString();
            QString teacher = obj["teacher"].toString();
            QString area = obj["area"].toString();
            QString building = obj["building"].toString();
            QString room = obj["room"].toString();

            if (section >= 0 && section <= 4) {
                QStringList locationParts;
                if (!area.isEmpty()) locationParts << area;
                if (!building.isEmpty()) locationParts << building;
                if (!room.isEmpty()) locationParts << room;
                QString location = locationParts.join("-");

                QString displayText = courseName + " " + teacher;
                if (!location.isEmpty()) displayText += "\n" + location;

                fixedResult[section] = displayText;
            }
        }
    }

    return fixedResult;
}

QVector<QStringList> DatabaseManager::parseCourseDataFromJson(const QJsonArray& jsonArray)
{
    QVector<QStringList> courseData;

    for (const auto& value : jsonArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            QStringList row;
            row << obj["week_day"].toString()
                << obj["section"].toString()
                << obj["course_name"].toString()
                << obj["teacher"].toString()
                << obj["area"].toString()
                << obj["building"].toString()
                << obj["room"].toString();
            courseData.append(row);
        }
    }

    return courseData;
}

// 计算当前周和日的工具方法
QPair<int, int> DatabaseManager::calculateWeekAndDay(const QDate& startDate)
{
    if (!startDate.isValid()) {
        return qMakePair(-1, -1);
    }

    QDate currentDate = QDate::currentDate();
    int totalDays = startDate.daysTo(currentDate);
    if (totalDays < 0) {
        return qMakePair(-1, -1);
    }

    int startDayOfWeek = startDate.dayOfWeek();
    int totalOffset = totalDays + startDayOfWeek - 1;
    int curr_week = totalOffset / 7 + 1;
    int curr_day = totalOffset % 7 + 1;
    if (curr_day == 7) curr_day = 0;

    return qMakePair(curr_week, curr_day);
}

// 具体的查询方法实现
void DatabaseManager::getClassSchedule(std::function<void(const QStringList&)> callback)
{
    QString query = "SELECT DISTINCT major FROM class_schedule ORDER BY major ASC";
    executeDistinctQuery(query, {}, callback);
}

void DatabaseManager::getClassSchedule(const QString &major, std::function<void(const QStringList&)> callback)
{
    QString query = "SELECT DISTINCT education_level FROM class_schedule WHERE major = ? ORDER BY education_level ASC";
    executeDistinctQuery(query, {major}, callback);
}

void DatabaseManager::getClassSchedule(const QString &major, const QString &educationLevel,
                                       std::function<void(const QStringList&)> callback)
{
    QString query = "SELECT DISTINCT grade FROM class_schedule WHERE major = ? AND education_level = ? ORDER BY grade ASC";
    executeDistinctQuery(query, {major, educationLevel}, callback);
}

void DatabaseManager::getClassSchedule(const QString &major, const QString &educationLevel,
                                       const QString &grade, std::function<void(const QStringList&)> callback)
{
    QString query = "SELECT DISTINCT class_num FROM class_schedule WHERE major = ? AND education_level = ? AND grade = ? ORDER BY class_num ASC";
    executeDistinctQuery(query, {major, educationLevel, grade}, callback);
}

void DatabaseManager::getClassSchedule(const QString &major, const QString &educationLevel,
                                       const QString &grade, const QString &classNum,
                                       std::function<void(const QStringList&)> callback)
{
    QString query = "SELECT DISTINCT week_num FROM class_schedule WHERE major = ? AND education_level = ? AND grade = ? AND class_num = ? ORDER BY week_num ASC";
    executeDistinctQuery(query, {major, educationLevel, grade, classNum}, callback);
}

void DatabaseManager::getAreaItems(std::function<void(const QStringList&)> callback)
{
    QString query = "SELECT DISTINCT area FROM class_schedule WHERE area IS NOT NULL AND area != '' ORDER BY area ASC";
    executeDistinctQuery(query, {}, callback);
}

void DatabaseManager::getBuildingItems(const QString &area, std::function<void(const QStringList&)> callback)
{
    QString query = "SELECT DISTINCT building FROM class_schedule WHERE area = ? AND building IS NOT NULL AND building != '' ORDER BY building ASC";
    executeDistinctQuery(query, {area}, callback);
}

void DatabaseManager::getEmptyRoom(const QString &area, const QString &building,
                                   int week_num, int day_num, std::function<void(const QStringList&)> callback)
{
    QString query =
        "WITH "
        "all_sections AS ("
        "    SELECT 0 AS section UNION SELECT 1 UNION SELECT 2 UNION SELECT 3 UNION SELECT 4"
        "),"
        "all_rooms AS ("
        "    SELECT DISTINCT room "
        "    FROM class_schedule "
        "    WHERE building = ? AND area = ?"
        "),"
        "all_combinations AS ("
        "    SELECT room, section "
        "    FROM all_rooms "
        "    CROSS JOIN all_sections"
        "),"
        "scheduled_classes AS ("
        "    SELECT room, section "
        "    FROM class_schedule "
        "    WHERE week_num = ? "
        "    AND week_day = ? "
        "    AND area = ? "
        "    AND building = ?"
        ") "
        "SELECT ac.room, ac.section "
        "FROM all_combinations ac "
        "LEFT JOIN scheduled_classes sc ON ac.room = sc.room AND ac.section = sc.section "
        "WHERE sc.room IS NULL "
        "ORDER BY ac.room, ac.section";

    QString requestId = generateRequestId();

    registerCallback(requestId, [callback, this](const QJsonArray& results) {
        callback(processEmptyRoomResults(results));
    });

    executeQueryWithRetry(query, {
                                     building, area,
                                     QString::number(week_num), QString::number(day_num),
                                     area, building
                                 }, requestId);
}

void DatabaseManager::getOnlineString(std::function<void(const QString&)> callback)
{
    QString query = "SELECT val FROM number_people_online WHERE id = 1;";
    QString requestId = generateRequestId();

    registerCallback(requestId, [callback](const QJsonArray& results) {
        if (results.size() > 0 && results[0].isObject()) {
            QJsonObject obj = results[0].toObject();
            QString value = QString::number(obj["val"].toInt());
            callback(value);
        } else {
            callback("0");
        }
    });

    executeQueryWithRetry(query, {}, requestId);
}

void DatabaseManager::getStartDate(std::function<void(const QDate&)> callback)
{
    QString query = "SELECT start_date FROM start_date WHERE id = 1;";
    QString requestId = generateRequestId();

    registerCallback(requestId, [callback](const QJsonArray& results) {
        QDate date;
        if (results.size() > 0 && results[0].isObject()) {
            QJsonObject obj = results[0].toObject();
            QString dateString = obj["start_date"].toString();

            date = QDate::fromString(dateString, "yyyy-MM-dd");
            if (!date.isValid()) {
                date = QDate::fromString(dateString.left(10), "yyyy-MM-dd");
            }
        }
        callback(date.isValid() ? date : QDate());
    });

    executeQueryWithRetry(query, {}, requestId);
}

void DatabaseManager::getCurrDayClassSchedule(std::function<void(const QStringList&)> callback)
{
    getStartDate([this, callback](const QDate& startDate) {
        auto weekAndDay = calculateWeekAndDay(startDate);
        int curr_week = weekAndDay.first;
        int curr_day = weekAndDay.second;

        if (curr_week < 1 || curr_day < 0) {
            callback(QStringList(5, ""));
            return;
        }

        QMap<QString, QVariant> CacheMap = LocalCacheManager::getInstance()->getCachingDataMap();
        QString query = "SELECT section, course_name, teacher, area, building, room"
                        " FROM class_schedule"
                        " WHERE major = ?"
                        "   AND education_level = ?"
                        "   AND grade = ?"
                        "   AND class_num = ?"
                        "   AND week_num = ?"
                        "   AND week_day = ?;";

        QVariantList params = {
            CacheMap["major"].toString(),
            CacheMap["education_level"].toString(),
            CacheMap["grade"].toString(),
            CacheMap["class_num"].toString(),
            QString::number(curr_week),
            QString::number(curr_day)
        };

        QString requestId = generateRequestId();

        registerCallback(requestId, [callback, this](const QJsonArray& results) {
            callback(processCurrentDaySchedule(results));
        });

        executeQueryWithRetry(query, params, requestId);
    });
}

void DatabaseManager::getNoticeStringList(std::function<void(const QStringList&)> callback)
{
    QString query = "SELECT content FROM notices";
    executeDistinctQuery(query, {}, callback);
}

void DatabaseManager::getClassScheduleModel(const QString &major, const QString &educationLevel,
                                            const QString &grade, const QString &classNum,
                                            int weekNum, std::function<void(QPair<bool, bool>)> callback)
{
    QString query =
        "SELECT week_day, section, course_name, teacher, area, building, room "
        "FROM class_schedule "
        "WHERE major = ? "
        "  AND education_level = ? "
        "  AND grade = ? "
        "  AND class_num = ? "
        "  AND week_num = ? "
        "ORDER BY week_day, section";

    QString requestId = generateRequestId();

    registerCallback(requestId, [this, callback](const QJsonArray& results) {
        bool cloudSuccess = !results.isEmpty();
        bool cacheSuccess = false;

        if (cloudSuccess) {
            // 解析课程数据
            QVector<QStringList> courseData = parseCourseDataFromJson(results);

            // 调用LocalCacheManager方法存储数据
            cacheSuccess = LocalCacheManager::getInstance()->storeCourseData(courseData);
        }

        callback(qMakePair(cloudSuccess, cacheSuccess));
    });

    executeQueryWithRetry(query, {major, educationLevel, grade, classNum, QString::number(weekNum)}, requestId);
}

void DatabaseManager::addPeopleOnline()
{
    QString query = "UPDATE number_people_online SET val = val + 1 WHERE id = 1";
    QString requestId = generateRequestId();

    // 注册成功回调
    registerCallback(requestId, [](const QJsonArray&) {
        qDebug() << "Online count incremented successfully";
    });

    // 设置超时处理
    QTimer::singleShot(5000, [this, requestId]() {
        QMutexLocker locker(&m_callbackMutex);
        if (m_callbacks.contains(requestId)) {
            qDebug() << "addPeopleOnline timeout";
            m_callbacks.remove(requestId);
        }
    });

    executeQueryWithRetry(query, {}, requestId);
}

void DatabaseManager::prepareForExit()
{
    QString query = "UPDATE number_people_online SET val = val - 1 WHERE id = 1 AND val > 0";
    QString requestId = generateRequestId();

    // 注册成功回调
    registerCallback(requestId, [](const QJsonArray&) {
        qDebug() << "Online count decremented successfully";
    });

    // 设置超时处理
    QTimer::singleShot(5000, [this, requestId]() {
        QMutexLocker locker(&m_callbackMutex);
        if (m_callbacks.contains(requestId)) {
            qDebug() << "prepareForExit timeout";
            m_callbacks.remove(requestId);
        }
    });

    executeQueryWithRetry(query, {}, requestId);
}
