// ============================================================================
// 文件名: http_weather_provider.cpp
// 功能描述: HTTP 天气数据获取实现。
// ============================================================================

#include "http_weather_provider.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

HttpWeatherProvider::HttpWeatherProvider(QObject *parent) : QObject(parent) {}

void HttpWeatherProvider::start() {
    if (!m_manager) {
        m_manager = new QNetworkAccessManager(this);
        connect(m_manager, &QNetworkAccessManager::finished, this, &HttpWeatherProvider::onReplyFinished);

        m_timer = new QTimer(this);
        m_timer->setInterval(30 * 60 * 1000); // 30 分钟更新一次
        connect(m_timer, &QTimer::timeout, this, &HttpWeatherProvider::fetchWeather);
        m_timer->start();
    }
    fetchWeather(); // 启动时立即拉取一次
}

void HttpWeatherProvider::stop() {
    if (m_timer) m_timer->stop();
}

void HttpWeatherProvider::fetchWeather() {
    QUrl url("https://wttr.in/Guangzhou?format=j1");
    QNetworkRequest request(url);
    emit logMessage("正在拉取云端天气数据...");
    m_manager->get(request);
}

void HttpWeatherProvider::onReplyFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        emit logMessage("天气获取失败: " + reply->errorString());
        reply->deleteLater(); // 安全销毁
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater(); // 必须使用 deleteLater 避免野指针

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject rootObj = doc.object();
        if (rootObj.contains("current_condition")) {
            QJsonArray condArray = rootObj["current_condition"].toArray();
            if (!condArray.isEmpty()) {
                QJsonObject current = condArray[0].toObject();
                QString temp = current["temp_C"].toString();
                QString desc = "未知";
                if (current.contains("weatherDesc")) {
                    QJsonArray descArray = current["weatherDesc"].toArray();
                    if (!descArray.isEmpty()) {
                        desc = descArray[0].toObject()["value"].toString();
                    }
                }
                emit weatherUpdated(temp + "℃", desc);
                emit logMessage(QString("天气更新成功: %1 %2").arg(temp).arg(desc));
            }
        }
    }
}