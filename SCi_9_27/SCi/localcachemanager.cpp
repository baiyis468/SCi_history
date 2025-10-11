#include "localcachemanager.h"
#include <QSqlError>
#include <QDebug>
#include <QSqlRecord>
#include <QStandardItemModel>
#include <QStandardItem>
#include "colorpicker.h"

// 实现自定义删除器
void LocalCacheManager::Deleter::operator()(LocalCacheManager* ptr) const {delete ptr;}
// 初始化 unique_ptr
std::unique_ptr<LocalCacheManager, LocalCacheManager::Deleter> LocalCacheManager::instance = nullptr;

LocalCacheManager::LocalCacheManager(){}
LocalCacheManager::~LocalCacheManager(){ closeDatabase();}

LocalCacheManager* LocalCacheManager::getInstance()
{
    if (!instance)
    {
        instance = std::unique_ptr<LocalCacheManager, Deleter>(new LocalCacheManager());
    }
    return instance.get();
}

bool LocalCacheManager::openDatabase(const QString& dbName)
{
    if (initialized)     return db.isOpen();
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);

    if (!db.open())
    {
        qDebug() << "无法打开数据库:" << db.lastError().text();
        return false;
    }
    initialized = true;
    return createTables();
}

void LocalCacheManager::closeDatabase() {db.close();}
bool LocalCacheManager::isOpen() const {return db.isOpen();}

bool LocalCacheManager::createTables() {
    QSqlQuery query;

    // 创建缓存表
    query.exec("CREATE TABLE IF NOT EXISTS caching_table ("
               "major VARCHAR(50) , "
               "education_level VARCHAR(10) , "
               "grade VARCHAR(10) , "
               "class_num VARCHAR(10) , "
               "week_num INT , "
               "updated BOOLEAN  DEFAULT 0)");

    // 创建课程表（根据新结构）
    query.exec("CREATE TABLE IF NOT EXISTS courses_table ("
               "week_day INT NOT NULL, "
               "section INT NOT NULL, "
               "course_name VARCHAR(100) NOT NULL, "
               "teacher VARCHAR(50), "
               "area VARCHAR(20), "
               "building VARCHAR(20), "
               "room VARCHAR(20))");
    query.exec("CREATE TABLE IF NOT EXISTS to_days_table ("
               "id INT NOT NULL,"
               "to_days INT)");

    query.exec("SELECT COUNT(*) FROM to_days_table");
    if (query.next() && query.value(0).toInt() == 0)
    {
        query.exec("INSERT INTO to_days_table (id, to_days) VALUES (1, 0)");
    }
    return true;
}

// ========== 缓存表操作 ==========
bool LocalCacheManager::saveCachingData(const QString& major, const QString& educationLevel,
                                        const QString& grade, const QString& classNum, int weekNum, bool updated) {
    QSqlQuery query;
    query.prepare("INSERT INTO caching_table (major, education_level, grade, class_num, week_num, updated) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(major);
    query.addBindValue(educationLevel);
    query.addBindValue(grade);
    query.addBindValue(classNum);
    query.addBindValue(weekNum);
    query.addBindValue(updated);

    if (!query.exec()) {
        qDebug() << "保存缓存数据失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool LocalCacheManager::updateCachingData(const QString& major, const QString& educationLevel,
                                          const QString& grade, const QString& classNum, int weekNum, bool updated) {
    if (!clearCachingTable()) {
        return false;
    }
    return saveCachingData(major, educationLevel, grade, classNum, weekNum, updated);
}

bool LocalCacheManager::clearCachingTable() {
    QSqlQuery query;
    if (!query.exec("DELETE FROM caching_table")) {
        qDebug() << "清空缓存表失败:" << query.lastError().text();
        return false;
    }
    return true;
}

QMap<QString, QVariant> LocalCacheManager::getCachingDataMap()
{
    QMap<QString, QVariant> data;
    QSqlQuery query("SELECT * FROM caching_table LIMIT 1", db);
    if (query.next())
    {
        data["major"] = query.value("major");
        data["education_level"] = query.value("education_level");
        data["grade"] = query.value("grade").toString();
        data["class_num"] = query.value("class_num").toString();
        data["week_num"] = query.value("week_num");
        data["updated"] = query.value("updated");
    }
    return data;
}

// ========== 课程表操作 ==========
bool LocalCacheManager::insertCourse(int weekDay, int section, const QString& courseName,
                                     const QString& teacher, const QString& area,
                                     const QString& building, const QString& room) {
    QSqlQuery query;
    query.prepare("INSERT INTO courses_table (week_day, section, course_name, teacher, area, building, room) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(weekDay);
    query.addBindValue(section);
    query.addBindValue(courseName);
    query.addBindValue(teacher);
    query.addBindValue(area);
    query.addBindValue(building);
    query.addBindValue(room);

    if (!query.exec())
    {
        qDebug() << "插入课程数据失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool LocalCacheManager::storeCourseData(const QVector<QStringList>& courseData)
{
    qDebug() << "=== 开始存储课程表数据到本地数据库 ===";
    qDebug() << "接收到的课程数据条数:" << courseData.size();

    // 开启事务
    if (!db.isOpen() && !openDatabase()) {
        qDebug() << "数据库未打开且打开失败";
        return false;
    }
    if (!db.transaction()) {
        qDebug() << "开启事务失败:" << db.lastError().text();
        return false;
    }

    // 清空课程表
    if (!clearCoursesTable()) {
        db.rollback();
        return false;
    }

    // 存储前验证数据
    QMap<QPair<int, int>, int> storageConflicts;
    int validRecordCount = 0;

    for (int i = 0; i < courseData.size(); ++i) {
        const QStringList& row = courseData[i];

        if (row.size() < 7) {
            qDebug() << "错误: 数据行字段不足，需要7个字段，实际:" << row.size();
            continue;
        }

        int weekDay = row[0].toInt();
        int section = row[1].toInt();
        QString courseName = row[2];
        QString teacher = row[3];
        QString area = row[4];
        QString building = row[5];
        QString room = row[6];

        qDebug() << "存储记录" << i << ": weekDay=" << weekDay << "section=" << section
                 << "course=" << courseName;

        // 跟踪存储冲突
        QPair<int, int> key = qMakePair(weekDay, section);
        storageConflicts[key] = storageConflicts.value(key, 0) + 1;

        if (!insertCourse(weekDay, section, courseName, teacher, area, building, room)) {
            qDebug() << "插入课程失败，回滚事务";
            db.rollback();
            return false;
        }

        validRecordCount++;
    }

    // 分析存储冲突
    qDebug() << "=== 存储冲突分析 ===";
    int totalStorageConflicts = 0;
    for (auto it = storageConflicts.begin(); it != storageConflicts.end(); ++it) {
        if (it.value() > 1) {
            totalStorageConflicts++;
            qDebug() << "存储冲突 - week_day:" << it.key().first << "section:" << it.key().second
                     << "有" << it.value() << "个课程";
        }
    }
    qDebug() << "存储过程中发现的冲突位置数量:" << totalStorageConflicts;
    qDebug() << "有效存储记录数:" << validRecordCount;

    // 提交事务
    if (!db.commit()) {
        qDebug() << "提交事务失败:" << db.lastError().text();
        db.rollback();
        return false;
    }

    qDebug() << "成功存储课程表数据，共插入" << validRecordCount << "行";
    qDebug() << "=== 存储完成 ===";

    return true;
}

bool LocalCacheManager::clearCoursesTable() {
    QSqlQuery query;
    if (!query.exec("DELETE FROM courses_table")) {
        qDebug() << "清空课程表失败:" << query.lastError().text();
        return false;
    }
    return true;
}

QStandardItemModel* LocalCacheManager::getCoursesTableModel()
{
    qDebug() << "=== 开始从本地数据库构建课程表模型 ===";

    // 创建模型：5行(节数) x 7列(周一至周日)
    QStandardItemModel* model = new QStandardItemModel(5, 7);

    // 设置横表头 (周一至周日)
    QStringList horizontalHeaders;
    horizontalHeaders << "周一" << "周二" << "周三" << "周四" << "周五" << "周六" << "周日";
    model->setHorizontalHeaderLabels(horizontalHeaders);

    // 设置竖表头 (第一节至第五节)
    QStringList verticalHeaders;
    verticalHeaders << "第一节" << "第二节" << "第三节" << "第四节" << "第五节";
    model->setVerticalHeaderLabels(verticalHeaders);

    // 从数据库获取课程数据
    QSqlQuery query("SELECT * FROM courses_table", db);

    if (!query.exec()) {
        qDebug() << "数据库查询失败:" << query.lastError().text();
        return model;
    }

    qDebug() << "数据库查询成功，开始解析记录";

    // 使用映射来跟踪每个位置的课程
    QMap<QPair<int, int>, QList<QString>> courseMap; // <(row, col), courseList>
    QMap<QPair<int, int>, QList<QColor>> colorMap;   // <(row, col), colorList>
    QMap<QPair<int, int>, int> dbConflicts;

    int recordCount = 0;

    while (query.next()) {
        recordCount++;

        // 直接从数据库读取数据
        int weekDay = query.value("week_day").toInt();
        int section = query.value("section").toInt();
        QString courseName = query.value("course_name").toString();
        QString teacher = query.value("teacher").toString();
        QString area = query.value("area").toString();
        QString building = query.value("building").toString();
        QString room = query.value("room").toString();

        qDebug() << "数据库记录" << recordCount << ": weekDay=" << weekDay << "section=" << section
                 << "course=" << courseName << "teacher=" << teacher;

        // 跟踪数据库中的冲突
        QPair<int, int> dbKey = qMakePair(weekDay, section);
        dbConflicts[dbKey] = dbConflicts.value(dbKey, 0) + 1;

        // 构建显示文本
        QString displayText = courseName;
        if (!teacher.isEmpty()) {
            displayText += "\n" + teacher;
        }

        // 构建地点信息
        QString location;
        if (!area.isEmpty()) location += area;
        if (!building.isEmpty()) location += (location.isEmpty() ? "" : "-") + building;
        if (!room.isEmpty()) location += (location.isEmpty() ? "" : "-") + room;

        if (!location.isEmpty()) {
            displayText += "\n" + location;
        }

        // 计算模型中的位置
        int row = section;  // 直接使用原始 section 值
        if (section < 0 || section >= 5) {
            qDebug() << "无效的节次:" << section << "，使用默认节次0";
            row = 0;
        }

        int col = -1;
        // 直接映射：0=周日, 1=周一, 2=周二, ..., 6=周六
        if (weekDay >= 1 && weekDay <= 6)
        {
            col = weekDay - 1;  // 周一至周六：1->0, 2->1, ..., 6->5
        } else if (weekDay == 0)
        {
            col = 6;  // 周日放在最后一列
        } else
        {
            qDebug() << "无效的星期:" << weekDay << "，使用默认周一";
            col = 0;
        }

        // 确保坐标有效
        if (row >= 0 && row < 5 && col >= 0 && col < 7) {
            QPair<int, int> position = qMakePair(row, col);

            // 添加到课程列表
            if (!courseMap.contains(position)) {
                courseMap[position] = QList<QString>();
                colorMap[position] = QList<QColor>();
            }

            courseMap[position].append(displayText);
            colorMap[position].append(ColorPicker::colorFor(courseName));

            if (courseMap[position].size() > 1) {
                qDebug() << "显示冲突！位置(" << row << "," << col << ")已有"
                         << courseMap[position].size() - 1 << "个课程，新增:" << courseName;
            }
        } else {
            qDebug() << "坐标无效: 行=" << row << "列=" << col;
        }
    }

    // 分析数据库中的冲突
    qDebug() << "=== 数据库原始冲突分析 ===";
    int totalDbConflicts = 0;
    for (auto it = dbConflicts.begin(); it != dbConflicts.end(); ++it) {
        if (it.value() > 1) {
            totalDbConflicts++;
            qDebug() << "数据库原始冲突 - week_day:" << it.key().first << "section:" << it.key().second
                     << "有" << it.value() << "个课程";
        }
    }
    qDebug() << "数据库中发现的原始冲突位置数量:" << totalDbConflicts;

    // 现在将课程设置到模型中
    int displayConflictCount = 0;
    for (auto it = courseMap.begin(); it != courseMap.end(); ++it) {
        QPair<int, int> position = it.key();
        int row = position.first;
        int col = position.second;
        QList<QString> courses = it.value();
        QList<QColor> colors = colorMap.value(position);

        QString displayText;
        QColor bgColor;

        if (courses.size() == 1) {
            // 单个课程：正常显示
            displayText = courses.first();
            bgColor = colors.first();
        } else {
            // 多个课程：特殊格式显示
            displayConflictCount++;
            displayText = QString("【%1门课程】\n").arg(courses.size());
            for (int i = 0; i < courses.size(); ++i) {
                QString courseText = courses[i];
                // 只显示课程名（去掉教师和地点信息）
                QString courseNameOnly = courseText.split('\n').first();
                displayText += QString("• %1\n").arg(courseNameOnly);
            }
            displayText = displayText.trimmed();

            // 使用特殊的冲突颜色
            bgColor = QColor(255, 200, 200); // 浅红色背景
        }

        QStandardItem* item = new QStandardItem(displayText);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        item->setBackground(bgColor);
        item->setForeground(ColorPicker::getContrastTextColor(bgColor));

        // 设置工具提示，显示完整信息
        if (courses.size() > 1) {
            QString tooltip = "该时间段有以下课程：\n\n";
            for (int i = 0; i < courses.size(); ++i) {
                tooltip += QString("%1. %2\n").arg(i + 1).arg(courses[i].replace('\n', ' '));
            }
            item->setToolTip(tooltip);
        }

        model->setItem(row, col, item);
    }

    qDebug() << "课程表数据解析完成，共处理" << recordCount << "条记录";
    qDebug() << "数据库原始冲突:" << totalDbConflicts << "个位置";
    qDebug() << "显示冲突:" << displayConflictCount << "个位置";
    qDebug() << "=== 模型构建完成 ===";

    return model;
}

bool LocalCacheManager::updateToDays(int newValue)
{
    QSqlQuery query;
    query.prepare("UPDATE to_days_table SET to_days = ? WHERE id = 1");
    query.addBindValue(newValue);
    return query.exec();
}

int LocalCacheManager::getToDays()
{
    QSqlQuery query;
    // 查询 id = 1 的 to_days
    query.prepare("SELECT to_days FROM to_days_table WHERE id = 1");
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
