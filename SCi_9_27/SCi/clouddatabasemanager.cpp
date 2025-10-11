#include "clouddatabasemanager.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

CloudDatabaseManager::CloudDatabaseManager(QObject *parent)
    : QObject{parent},
    m_networkmanager(new QNetworkAccessManager(this))
{
    connect(m_networkmanager, &QNetworkAccessManager::finished, this, &CloudDatabaseManager::onQueryFinished);
}

CloudDatabaseManager::~CloudDatabaseManager(){}

void CloudDatabaseManager::executeQuery(const QString &query, const QVariantList &params, const QString &requestId)
{
    QUrl url(API_URL);
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-API-Key", API_KEY.toUtf8());

    QJsonObject json;
    json["query"] = query;

    // 生成或使用传入的 requestId
    QString actualRequestId = requestId;
    if (actualRequestId.isEmpty()) {
        actualRequestId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    json["requestId"] = actualRequestId;

    QJsonArray jsonParams;
    for (const QVariant &param : params) {
        jsonParams.append(QJsonValue::fromVariant(param));
    }
    json["params"] = jsonParams;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    m_networkmanager->post(request, data);
}

void CloudDatabaseManager::executeUpdate(const QString &query, const QVariantList &params, const QString &requestId)
{
    // 更新操作与查询操作使用相同的底层实现
    executeQuery(query, params, requestId);
}

void CloudDatabaseManager::onQueryFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject json = doc.object();

        // 从响应中获取 requestId
        QString requestId = json["requestId"].toString();

        if(json["success"].toBool())
        {
            emit connectionStatusChanged(true);

            // 判断是查询结果还是更新结果
            if(json.contains("data"))
            {
                QJsonArray data = json["data"].toArray();
                emit queryCompleted(requestId, data);
            }
            else if(json.contains("affected_rows"))
            {
                int affectedRows = json["affected_rows"].toInt();
                emit updateCompleted(requestId, affectedRows);

                // 同时发送文本结果（兼容旧代码）
                QString resultText = QString::number(affectedRows);
                emit resultReceived(requestId, resultText);
            }
        }
        else
        {
            emit connectionStatusChanged(false);
            QString errorMsg = json["error"].toString();
            emit queryError(requestId, errorMsg);
        }
    }
    else
    {
        // 网络错误
        emit connectionStatusChanged(false);
        QString errorStr = reply->errorString();
        qDebug() << "网络错误详情:" << errorStr;
        emit queryError("", "网络错误: " + errorStr);
    }
    reply->deleteLater();
}
