#include "clouddatabasemanager.h"

const QString CloudDatabaseManager::DB_HOST = "mysql2.sqlpub.com";
const QString CloudDatabaseManager::DB_NAME = "xiaoyuanapp";
const QString CloudDatabaseManager::DB_USER = "xiaoyuanapp";
//const QString CloudDatabaseManager::DB_PASSWORD = "";
const int CloudDatabaseManager::DB_PORT = 3307;

CloudDatabaseManager::CloudDatabaseManager(QObject *parent)
    : QObject{parent}
{
    db = QSqlDatabase::addDatabase("QMYSQL", QString::number(reinterpret_cast<quintptr>(this)));
    db.setHostName(DB_HOST);
    db.setDatabaseName(DB_NAME);
    db.setUserName(DB_USER);
    db.setPassword(DB_PASSWORD);
    db.setPort(DB_PORT);
}

CloudDatabaseManager::~CloudDatabaseManager()
{
    closeDatabase();
}

bool CloudDatabaseManager::openDatabase()
{
    if (db.isOpen()) return true;

    if (!db.open()) {
        qDebug() << "数据库打开错误:" << db.lastError().text();
        return false;
    }
    return true;
}

void CloudDatabaseManager::closeDatabase()
{
    db.close();
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
        qDebug() << "Query error:" << query.lastError().text() << "\nSQL:" << sql;
    }
    return query;
}

bool CloudDatabaseManager::executeUpdate(const QString &sql, const QVariantList &params)
{
    QSqlQuery query = executeQuery(sql, params);
    return query.numRowsAffected() > 0;
}
