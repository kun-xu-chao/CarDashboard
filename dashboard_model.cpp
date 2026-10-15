// ============================================================================
// 文件名: dashboard_model.cpp
// 功能描述: 仪表盘数据模型的实现，采用按需刷新策略（Diff-then-Emit）。
// ============================================================================

#include "dashboard_model.h"

DashboardModel::DashboardModel(QObject *parent) : QObject(parent) {}

void DashboardModel::updateData(const VehicleData& data) {
    // 逐字段对比，仅在数据真正变化时发出 NOTIFY 信号，避免高帧率下 QML 无谓重绘
    if (m_data.speed != data.speed) { m_data.speed = data.speed; emit speedChanged(); }
    if (m_data.rpm != data.rpm) { m_data.rpm = data.rpm; emit rpmChanged(); }
    if (m_data.coolantTemp != data.coolantTemp) { m_data.coolantTemp = data.coolantTemp; emit coolantTempChanged(); }
    if (m_data.batterySoc != data.batterySoc) { m_data.batterySoc = data.batterySoc; emit batterySocChanged(); }

    if (m_data.leftTurnLight != data.leftTurnLight) { m_data.leftTurnLight = data.leftTurnLight; emit leftTurnLightChanged(); }
    if (m_data.highBeam != data.highBeam) { m_data.highBeam = data.highBeam; emit highBeamChanged(); }
    if (m_data.seatBelt != data.seatBelt) { m_data.seatBelt = data.seatBelt; emit seatBeltChanged(); }
}

// MQTT 局部更新入口，不触碰其他字段，避免覆盖串口模拟数据
void DashboardModel::updateSpeed(float speed) {
    if (m_data.speed != speed) {
        m_data.speed = speed;
        emit speedChanged();
    }
}

void DashboardModel::updateOtaProgress(int progress) {
    if (m_otaProgress != progress) {
        m_otaProgress = progress;
        emit otaProgressChanged();
    }
}

void DashboardModel::updateWeather(const QString& temp, const QString& desc) {
    if (m_weatherTemp != temp || m_weatherDesc != desc) {
        m_weatherTemp = temp;
        m_weatherDesc = desc;
        emit weatherChanged();
    }
}