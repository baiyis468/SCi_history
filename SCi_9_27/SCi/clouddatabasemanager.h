#ifndef CLOUDDATABASEMANAGER_H
#define CLOUDDATABASEMANAGER_H

#include <QObject>
#include <QSqlQuery>
#include <QNetworkAccessManager>

class CloudDatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit CloudDatabaseManager(QObject *parent = nullptr);
    virtual ~CloudDatabaseManager();

    // 执行查询语句，可选的requestId参数用于标识请求
    void executeQuery(const QString &query,
                      const QVariantList &params = QVariantList(),
                      const QString &requestId = QString());

    // 执行更新语句，可选的requestId参数用于标识请求
    void executeUpdate(const QString &query,
                       const QVariantList &params = QVariantList(),
                       const QString &requestId = QString());

private slots:
    void onQueryFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager* m_networkmanager;
    const QString API_URL = "https://xxxx/api.php";
    const QString API_KEY = "xxxxx";
    //请填写您自己API的 URL 和 KEY

signals:
    void connectionStatusChanged(bool isConnected);
    void queryCompleted(const QString &requestId, const QJsonArray &results);
    void updateCompleted(const QString &requestId, int affectedRows);
    void queryError(const QString &requestId, const QString &error);
    void resultReceived(const QString &requestId, const QString &result);
};

#endif // CLOUDDATABASEMANAGER_H
