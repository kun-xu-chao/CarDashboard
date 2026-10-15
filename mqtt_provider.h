// ============================================================================
// 文件名: mqtt_provider.h
// 功能描述: MQTT 车云通信客户端 (运行于独立子线程)。
// ============================================================================

#pragma once
#include <QObject>
#include <QMqttClient>
#include "idata_provider.h"

class MqttProvider : public IDataProvider {
    Q_OBJECT
public:
    explicit MqttProvider(QObject *parent = nullptr);
    void start() override;
    void stop() override;

signals:
    void logMessage(const QString& msg);
    void speedUpdated(float speed);         // 局部更新信号
    void otaProgressUpdated(int progress);  // OTA 进度信号

private slots:
    void onConnected();
    void onMessageReceived(const QByteArray &message, const QMqttTopicName &topic);

private:
    // 延迟初始化 (在 start() 中创建)，确保网络对象归属子线程
    QMqttClient *m_client = nullptr;
};