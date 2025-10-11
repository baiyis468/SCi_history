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

    if (!query.exec()) {
        qDebug() << "插入课程数据失败:" << query.lastError().text();
        return false;
    }
    return true;
}

QColor LocalCacheManager::getContrastTextColor(const QColor &bgColor)
{
    int brightness = qGray(bgColor.rgb());
    return brightness > 128 ? Qt::black : Qt::white;
}

bool LocalCacheManager::replaceCoursesTable(QSqlQueryModel* model) {
    if (!model) {
        qDebug() << "传入的模型为空指针";
        return false;
    }
    // 开启事务
    if (!db.isOpen() && !openDatabase())
    {
        qDebug() << "数据库未打开且打开失败";
        return false;
    }
    if (!db.transaction())
    {
        qDebug() << "开启事务失败:" << db.lastError().text();
        return false;
    }
    // 清空课程表
    if (!clearCoursesTable()) {
        db.rollback();
        return false;
    }

    const int rowCount = model->rowCount();
    const int colCount = model->columnCount();

    // 确保列数匹配（需要7列）
    if (colCount < 7) {
        qDebug() << "列数不足，实际列数:" << colCount;
        db.rollback();
        return false;
    }

    for (int row = 0; row < rowCount; ++row) {
        int weekDay = model->data(model->index(row, 0)).toInt();
        int section = model->data(model->index(row, 1)).toInt();
        QString courseName = model->data(model->index(row, 2)).toString();
        QString teacher = model->data(model->index(row, 3)).toString();
        QString area = model->data(model->index(row, 4)).toString();
        QString building = model->data(model->index(row, 5)).toString();
        QString room = model->data(model->index(row, 6)).toString();

        if (!insertCourse(weekDay, section, courseName, teacher, area, building, room)) {
            db.rollback();
            return false;
        }
    }

    // 提交事务
    if (!db.commit())
    {
        qDebug() << "提交事务失败:" << db.lastError().text();
        db.rollback();
        return false;
    }

    qDebug() << "成功替换课程表数据，共插入" << rowCount << "行";
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
    while (query.next()) {
        // 解析数据库字段
        int weekDay = query.value("week_day").toInt();
        int section = query.value("section").toInt();
        QString courseName = query.value("course_name").toString();
        QString teacher = query.value("teacher").toString();
        QString area = query.value("area").toString();
        QString building = query.value("building").toString();
        QString room = query.value("room").toString();

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
        int row = section;    // 节数直接对应行索引 (0-4)
        int col = weekDay - 1; // 转换week_day: 1->0(周一), 2->1(周二)...

        // 特殊处理周日 (week_day=0 转换为第6列)
        if (weekDay == 0) {
            col = 6; // 周日放在最后一列
        }

        // 确保坐标有效
        if (row >= 0 && row < 5 && col >= 0 && col < 7) {
            QStandardItem* item = new QStandardItem(displayText);
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            QColor bgColor = ColorPicker::colorFor(courseName);
            item->setBackground(bgColor);
            item->setForeground(getContrastTextColor(bgColor));
            model->setItem(row, col, item);
        }
        else qDebug("坐标无效！");
    }
    return model;
}
