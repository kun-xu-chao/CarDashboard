// ============================================================================
// 文件名: mqtt_provider.cpp
// 功能描述: MQTT 车云通信客户端实现。
// ============================================================================

#include "mqtt_provider.h"
#include <QJsonDocument>
#include <QJsonObject>

MqttProvider::MqttProvider(QObject *parent) : IDataProvider(parent) {
    // 构造函数保持空白，避免在主线程创建 QMqttClient 导致跨线程崩溃
}

void MqttProvider::start() {
    if (!m_client) {
        // 延迟初始化：在子线程中创建网络对象，满足 Qt 线程亲和性要求
        m_client = new QMqttClient(this);
        m_client->setHostname("bemfa.com");
        m_client->setPort(9501);
        m_client->setClientId("cdd2151334ed2b553a4a6b9c7d1ca748");

        connect(m_client, &QMqttClient::connected, this, &MqttProvider::onConnected);
        connect(m_client, &QMqttClient::messageReceived, this, &MqttProvider::onMessageReceived);

        // 状态监听
        connect(m_client, &QMqttClient::stateChanged, this, [this](QMqttClient::ClientState state) {
            emit logMessage(QString("MQTT 状态改变: %1").arg(state));
        });
    }

    if (m_client->state() == QMqttClient::Disconnected) {
        m_client->connectToHost();
        emit logMessage("正在连接 MQTT 服务器 (巴法云)...");
    }
}

void MqttProvider::stop() {
    if (m_client && m_client->state() == QMqttClient::Connected) {
        m_client->disconnectFromHost();
        emit logMessage("已断开 MQTT 连接。");
    }
}

void MqttProvider::onConnected() {
    emit logMessage("MQTT 连接成功！开始订阅云端指令...");
    m_client->subscribe(QMqttTopicFilter("B5fTFS050002"), 0);
}

void MqttProvider::onMessageReceived(const QByteArray &message, const QMqttTopicName &topic) {
    emit logMessage("收到云端消息: " + QString::fromUtf8(message));

    QJsonDocument doc = QJsonDocument::fromJson(message);
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject obj = doc.object();

        // 采用细粒度信号发送，避免构造残缺的全量 VehicleData 覆盖串口数据
        if (obj.contains("speed")) {
            emit speedUpdated(obj["speed"].toDouble());
        }
        if (obj.contains("ota_progress")) {
            int progress = obj["ota_progress"].toInt();
            emit otaProgressUpdated(progress);
            emit logMessage(QString("收到云端 OTA 升级进度: %1%").arg(progress));
        }
    }
}