// ============================================================================
// 文件名: simulator.h
// 功能描述: CAN 数据模拟器（发送端）。
// ============================================================================

#pragma once
#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include "idata_provider.h"

class Simulator : public IDataProvider {
    Q_OBJECT
public:
    explicit Simulator(QObject *parent = nullptr);

    Q_INVOKABLE void start() override;
    Q_INVOKABLE void stop() override;

signals:
    void logMessage(const QString& msg);

private slots:
    void generateAndSend();

private:
    // 辅助函数：消除重复的拼帧逻辑（DRY 原则）
    QByteArray buildFrame(uint32_t canId, const uint8_t* data);

    QSerialPort *m_serial;
    QTimer      *m_timer;
    float        m_angle;
    int          m_sendLogCounter = 0;
};