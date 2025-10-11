#include "clouddatabasemanager.h"
#include<QTimer>


CloudDatabaseManager::CloudDatabaseManager(QObject *parent)
    : QObject{parent}
{
    timer = new QTimer(this);
    timer->setSingleShot(true); // 设置为单次触发,为了没有open操作下不会一直触发closeAndRemoveConnection。
    connect(timer, &QTimer::timeout, this, [this]() {
        if (!m_aboutToQuit) { // 仅在非退出状态关闭连接
            closeAndRemoveConnection();
        }
    });
    connect(this, &CloudDatabaseManager::open, this, [this]() {
        if (!m_aboutToQuit) timer->start(10000);
    });
    timer->start(10000);

}
CloudDatabaseManager::~CloudDatabaseManager()
{
    closeAndRemoveConnection();
}

bool CloudDatabaseManager::openDatabase()
{
    // 先关闭并移除现有连接
    closeAndRemoveConnection();
    emit open();
    // 创建新的连接（使用唯一连接名）
    db = QSqlDatabase::addDatabase("QMYSQL", "connection_sql");
    db.setHostName(DB_HOST);
    db.setDatabaseName(DB_NAME);
    db.setUserName(DB_USER);
    db.setPassword(DB_PASSWORD);
    db.setPort(DB_PORT);

    // 尝试打开连接
    if (!db.open())
    {
        qDebug() << "数据库打开错误:" << db.lastError().text();
        return false;
    }
    return true;
}

void CloudDatabaseManager::prepareForShutdown()
{
    m_aboutToQuit = true;
    if (timer)
    {
        timer->stop();
        delete timer;
        timer = nullptr;
    }
    closeAndRemoveConnection();
}



QSqlQuery CloudDatabaseManager::executeQuery(const QString &sql, const QVariantList &params)
{
    if (!db.isOpen() && !openDatabase())
    {
        qDebug() << "数据库没有在查询时打开:" << sql;
        return QSqlQuery();
    }
    QSqlQuery query(db);
    query.prepare(sql);
    for (int i = 0; i < params.size(); ++i)
    {
        query.bindValue(i, params[i]);
    }
    if (!query.exec())
    {
        qDebug() << "查询错误: " << query.lastError().text() << "\nSQL:" << sql;
    }
    return query;
}

bool CloudDatabaseManager::executeUpdate(const QString &sql, const QVariantList &params)
{
    QSqlQuery query = executeQuery(sql, params);
    return query.numRowsAffected() > 0;
}

void CloudDatabaseManager::closeAndRemoveConnection()
{
    if (db.isValid())
    {
        QString connectionName = db.connectionName();
        db.close();
        db = QSqlDatabase();
        if (!connectionName.isEmpty())     QSqlDatabase::removeDatabase(connectionName);
    }
}
