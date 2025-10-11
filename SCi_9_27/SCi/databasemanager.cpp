#include "databasemanager.h"
#include "localcachemanager.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QCollator>
#include <QNetworkReply>
#include <QRandomGenerator>
#include <QDateTime>
#include<QPointer>
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
    qDebug() << "执行回调函数:" << requestId;

    // 先提取回调函数，然后释放锁
    std::function<void(const QJsonArray&)> callback;
    {
        QMutexLocker locker(&m_callbackMutex);
        if (m_callbacks.contains(requestId)) {
            callback = m_callbacks.take(requestId).second;
            qDebug() << "回调函数已找到";
        } else {
            qDebug() << "没有找到请求的回调:" << requestId;
            return;
        }
    }

    // 在锁外执行回调，避免死锁
    if (callback) {
        qDebug() << "开始执行回调...";
        try {
            callback(results);
            qDebug() << "回调执行成功！";
        } catch (const std::exception& e) {
            qDebug() << "回调执行出现异常:" << e.what();
        } catch (...) {
            qDebug() << "回调执行出现未知异常";
        }
    } else {
        qDebug() << "回调函数无效";
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
                QJsonValue val = obj.contains(columnName) ? obj[columnName] : obj.begin().value();

                // 核心修复：统一处理数字类型转换
                list.append(val.isDouble() ? QString::number(val.toInt()) : val.toString());
            } else {
                // 处理非对象情况
                list.append(value.isDouble() ? QString::number(value.toInt()) : value.toString());
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

    // 设置更合理的超时重试（从5秒改为10秒）
    QTimer::singleShot(10000, [=]() {  // 修改这里：5000 -> 10000
        QMutexLocker locker(&m_callbackMutex);
        if (m_callbacks.contains(requestId)) {
            qDebug() << "查询超时，重试查询:" << requestId << "剩余重试次数:" << (retries - 1);
            executeQueryWithRetry(query, params, requestId, retries - 1);
        }
    });
}

void DatabaseManager::onQueryCompleted(const QString &requestId, const QJsonArray &results)
{
    qDebug() << "查询完成:" << requestId << "结果总数:" << results.size();
    executeCallback(requestId, results);
}

void DatabaseManager::onQueryError(const QString &requestId, const QString &error)
{
    qDebug() << "查询错误:" << requestId << "  错误:" << error;
    QMutexLocker locker(&m_callbackMutex);
    if (m_callbacks.contains(requestId)) {
        m_callbacks.remove(requestId);
        emit queryFailed(error);
    }
}

void DatabaseManager::onConnectionStatusChanged(bool connected)
{
    qDebug() << "数据库连接状态变化:" << connected;
    // 添加延迟确保状态稳定
    QTimer::singleShot(100, [this, connected]() {
        emit connectionStatusChanged(connected);
    });
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
    qDebug() << "processCurrentDaySchedule 开始，结果数量:" << results.size();

    QStringList fixedResult(5, "");

    for (int i = 0; i < results.size(); ++i) {
        const auto& value = results[i];
        qDebug() << "处理第" << i << "个结果";

        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            qDebug() << "结果对象键值:" << obj.keys();

            int section = obj["section"].toInt();
            QString courseName = obj["course_name"].toString();
            QString teacher = obj["teacher"].toString();

            qDebug() << "section:" << section << "course:" << courseName;

            if (section >= 0 && section <= 4) {
                // 确保数据安全
                if (courseName.isEmpty()) courseName = "未知课程";
                if (teacher.isEmpty()) teacher = "未知教师";

                fixedResult[section] = courseName + " " + teacher;
                qDebug() << "设置第" << section << "节课:" << fixedResult[section];
            }
        } else {
            qDebug() << "第" << i << "个结果不是对象";
        }
    }

    qDebug() << "processCurrentDaySchedule 完成";
    return fixedResult;
}

QVector<QStringList> DatabaseManager::parseCourseDataFromJson(const QJsonArray& jsonArray)
{
    QVector<QStringList> courseData;
    qDebug() << "=== 开始详细分析JSON课程数据 ===";
    qDebug() << "原始JSON记录数:" << jsonArray.size();

    // 用于跟踪原始数据的冲突
    QMap<QPair<int, int>, QList<QJsonObject>> rawDataConflicts;

    for (int i = 0; i < jsonArray.size(); ++i) {
        const auto& value = jsonArray[i];
        if (value.isObject()) {
            QJsonObject obj = value.toObject();

            // 提取原始数据（不进行任何调整）
            int originalWeekDay = obj["week_day"].toInt();
            int originalSection = obj["section"].toInt();
            QString courseName = obj["course_name"].toString();
            QString teacher = obj["teacher"].toString();
            QString area = obj["area"].toString();
            QString building = obj["building"].toString();
            QString room = obj["room"].toString();

            qDebug() << "--- 记录" << i << "分析 ---";
            qDebug() << "原始week_day:" << originalWeekDay << "原始section:" << originalSection;
            qDebug() << "课程名称:" << courseName;
            qDebug() << "教师:" << teacher;
            qDebug() << "区域:" << area << "建筑:" << building << "房间:" << room;

            // 检查是否是有效数据（非空课程名）
            if (courseName.isEmpty() || courseName.trimmed().isEmpty()) {
                qDebug() << "警告: 课程名称为空，跳过此记录";
                continue;
            }

            // 检查是否是特殊记录（如"班"、"呵1（男）"等）
            if (courseName.contains("班") || courseName.contains("呵") ||
                courseName.contains("徘") || courseName.contains("怖")) {
                qDebug() << "注意: 检测到特殊课程记录，可能是班级活动或体育课";
            }

            // 记录原始数据的冲突
            QPair<int, int> rawKey = qMakePair(originalWeekDay, originalSection);
            if (!rawDataConflicts.contains(rawKey)) {
                rawDataConflicts[rawKey] = QList<QJsonObject>();
            }
            rawDataConflicts[rawKey].append(obj);

            // 修改这里：直接使用原始值，不进行任何调整
            int weekDay = originalWeekDay;
            int section = originalSection;


            QStringList row;
            row << QString::number(weekDay)
                << QString::number(section)
                << courseName
                << teacher
                << area
                << building
                << room;
            courseData.append(row);

            qDebug() << "保持原样: weekDay=" << weekDay << "section=" << section;
        }
    }

    // 分析原始数据的冲突情况
    qDebug() << "=== 原始数据冲突分析 ===";
    int totalRawConflicts = 0;
    for (auto it = rawDataConflicts.begin(); it != rawDataConflicts.end(); ++it) {
        if (it.value().size() > 1) {
            totalRawConflicts++;
            QPair<int, int> key = it.key();
            qDebug() << "原始数据冲突 - week_day:" << key.first << "section:" << key.second
                     << "有" << it.value().size() << "个课程:";
            for (int i = 0; i < it.value().size(); ++i) {
                QJsonObject obj = it.value()[i];
                qDebug() << "  " << (i+1) << ". " << obj["course_name"].toString()
                         << "(" << obj["teacher"].toString() << ")";
            }
        }
    }

    qDebug() << "原始数据中发现的冲突位置数量:" << totalRawConflicts;
    qDebug() << "最终解析的记录数:" << courseData.size();
    qDebug() << "=== JSON数据解析完成 ===";

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

    // 添加调试输出
    qDebug() << "查询届数据 - 专业:" << major << "层次:" << educationLevel;
    qDebug() << "SQL:" << query;

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
    qDebug() << "getCurrDayClassSchedule 开始";

    getStartDate([this, callback](const QDate& startDate) {
        if (!startDate.isValid()) {
            qDebug() << "起始日期无效，返回空课表";
            callback(QStringList(5, ""));
            return;
        }

        qDebug() << "获取到起始日期:" << startDate.toString("yyyy-MM-dd");

        auto weekAndDay = calculateWeekAndDay(startDate);
        int curr_week = weekAndDay.first;
        int curr_day = weekAndDay.second;

        qDebug() << "计算出的周和日:" << curr_week << curr_day;

        if (curr_week < 1 || curr_day < 0) {
            qDebug() << "周或日无效，返回空课表";
            callback(QStringList(5, ""));
            return;
        }

        QMap<QString, QVariant> CacheMap = LocalCacheManager::getInstance()->getCachingDataMap();
        qDebug() << "缓存数据检查:";
        qDebug() << "major:" << CacheMap["major"].toString();
        qDebug() << "education_level:" << CacheMap["education_level"].toString();
        qDebug() << "grade:" << CacheMap["grade"].toString();
        qDebug() << "class_num:" << CacheMap["class_num"].toString();

        // 检查缓存数据是否完整
        if (CacheMap["major"].toString().isEmpty() ||
            CacheMap["education_level"].toString().isEmpty() ||
            CacheMap["grade"].toString().isEmpty() ||
            CacheMap["class_num"].toString().isEmpty()) {
            qDebug() << "缓存数据不完整，返回空课表";
            callback(QStringList(5, ""));
            return;
        }

        QString query = "SELECT section, course_name, teacher, area, building, room "
                        "FROM class_schedule "
                        "WHERE major = ? "
                        "AND education_level = ? "
                        "AND grade = ? "
                        "AND class_num = ? "
                        "AND week_num = ? "
                        "AND week_day = ?;";

        QVariantList params = {
            CacheMap["major"].toString(),
            CacheMap["education_level"].toString(),
            CacheMap["grade"].toString(),
            CacheMap["class_num"].toString(),
            QString::number(curr_week),
            QString::number(curr_day)
        };

        qDebug() << "执行今日课程查询，参数:" << params;

        QString requestId = generateRequestId();

        registerCallback(requestId, [callback, this](const QJsonArray& results) {
            qDebug() << "今日课程查询完成，结果数量:" << results.size();
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
        qDebug() << "在线计数成功递增";
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
