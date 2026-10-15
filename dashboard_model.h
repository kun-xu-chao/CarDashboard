// ============================================================================
// 文件名: dashboard_model.h
// 功能描述: 仪表盘数据模型 (ViewModel)，C++ 与 QML 交互的桥梁。
// ============================================================================

#pragma once
#include <QObject>
#include "vehicle_data.h"

class DashboardModel : public QObject {
    Q_OBJECT

    // 连续物理量
    Q_PROPERTY(float speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(float rpm READ rpm NOTIFY rpmChanged)
    Q_PROPERTY(float coolantTemp READ coolantTemp NOTIFY coolantTempChanged)
    Q_PROPERTY(float batterySoc READ batterySoc NOTIFY batterySocChanged)

    // 离散开关量
    Q_PROPERTY(bool leftTurnLight READ leftTurnLight NOTIFY leftTurnLightChanged)
    Q_PROPERTY(bool highBeam READ highBeam NOTIFY highBeamChanged)
    Q_PROPERTY(bool seatBelt READ seatBelt NOTIFY seatBeltChanged)

    // 车云网联
    Q_PROPERTY(int otaProgress READ otaProgress NOTIFY otaProgressChanged)
    Q_PROPERTY(QString weatherTemp READ weatherTemp NOTIFY weatherChanged)
    Q_PROPERTY(QString weatherDesc READ weatherDesc NOTIFY weatherChanged)

public:
    explicit DashboardModel(QObject *parent = nullptr);

    // Q_PROPERTY 的 READ 访问器
    float speed() const { return m_data.speed; }
    float rpm() const { return m_data.rpm; }
    float coolantTemp() const { return m_data.coolantTemp; }
    float batterySoc() const { return m_data.batterySoc; }

    bool leftTurnLight() const { return m_data.leftTurnLight; }
    bool highBeam() const { return m_data.highBeam; }
    bool seatBelt() const { return m_data.seatBelt; }

    int otaProgress() const { return m_otaProgress; }
    QString weatherTemp() const { return m_weatherTemp; }
    QString weatherDesc() const { return m_weatherDesc; }

public slots:
    void updateData(const VehicleData& data); // 串口全量数据更新
    void updateSpeed(float speed);            // MQTT 局部车速更新
    void updateOtaProgress(int progress);     // MQTT OTA 进度更新
    void updateWeather(const QString& temp, const QString& desc); // HTTP 天气更新

signals:
    void speedChanged();
    void rpmChanged();
    void coolantTempChanged();
    void batterySocChanged();
    void leftTurnLightChanged();
    void highBeamChanged();
    void seatBeltChanged();
    void otaProgressChanged();
    void weatherChanged();
    void logMessage(const QString& msg);

private:
    VehicleData m_data;           // 车辆数据快照
    int m_otaProgress = 0;        // OTA 进度
    QString m_weatherTemp = "--"; // 天气温度
    QString m_weatherDesc = "加载中"; // 天气描述
};