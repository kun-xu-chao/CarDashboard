// ============================================================================
// 文件名: http_weather_provider.h
// 功能描述: HTTP 天气数据获取 (运行于独立子线程)。
// ============================================================================

#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

class HttpWeatherProvider : public QObject {
    Q_OBJECT
public:
    explicit HttpWeatherProvider(QObject *parent = nullptr);
    void start();
    void stop();

signals:
    void weatherUpdated(const QString& temperature, const QString& description);
    void logMessage(const QString& msg);

private slots:
    void fetchWeather();
    void onReplyFinished(QNetworkReply *reply);

private:
    // 延迟初始化，确保 QNetworkAccessManager 归属子线程
    QNetworkAccessManager *m_manager = nullptr;
    QTimer *m_timer = nullptr;
};