#ifndef CLOUDDATABASEMANAGER_H
#define CLOUDDATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class CloudDatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit CloudDatabaseManager(QObject *parent = nullptr);
    virtual ~CloudDatabaseManager();

    virtual bool openDatabase();
    void closeDatabase();

protected:
    QSqlQuery executeQuery(const QString &sql, const QVariantList &params = QVariantList());//执行查询语句
    bool executeUpdate(const QString &sql, const QVariantList &params = QVariantList());//执行非查询语句
            // 两个参数分别是sql语句和 sql里面的填充的占位符
    QSqlDatabase db;
    static const QString DB_HOST;
    static const QString DB_NAME;
    static const QString DB_USER;
    static const QString DB_PASSWORD;
    static const int DB_PORT;
};

#endif // CLOUDDATABASEMANAGER_H
