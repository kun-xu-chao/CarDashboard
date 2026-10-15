// ============================================================================
// 文件名: simulator.cpp
// 功能描述: CAN 数据模拟器的具体实现。
// ============================================================================

#include "simulator.h"
#include <QtMath>
#include <QDebug>

Simulator::Simulator(QObject *parent) : IDataProvider(parent) {
    m_serial = new QSerialPort(this);
    m_serial->setPortName("COM1");
    m_serial->setBaudRate(QSerialPort::Baud115200);

    m_timer = new QTimer(this);
    m_timer->setInterval(50); // 20Hz 刷新率
    connect(m_timer, &QTimer::timeout, this, &Simulator::generateAndSend);
}

void Simulator::start() {
    if (m_timer->isActive()) return; // 幂等启动，防止定时器叠加

    if (!m_serial->isOpen()) {
        if (!m_serial->open(QIODevice::WriteOnly)) {
            emit logMessage("错误：无法打开 COM1！");
            return;
        }
    } else {
        m_serial->clear(); // 清理残留缓冲区
    }

    m_angle = 0.0f;
    m_sendLogCounter = 0;
    m_timer->start();
    emit logMessage("模拟器已启动，正在向 COM1 发送数据...");
}

void Simulator::stop() {
    m_timer->stop();
    if (m_serial->isOpen()) {
        m_serial->clear();
        m_serial->close();
    }
    emit logMessage("模拟器已停止。");
}

void Simulator::generateAndSend() {
    // 1. 生成物理值 (正弦波平滑变化)
    m_angle += 0.05f;
    float speed   = 60.0f + 40.0f * qSin(m_angle);
    float rpm     = 2000.0f + 1500.0f * qSin(m_angle * 2);
    float coolant = 85.0f + 10.0f * qSin(m_angle * 0.5);
    float battery = 75.0f + 15.0f * qCos(m_angle * 0.3);

    // 2. DBC 编码 (物理值 -> 原始值)
    uint16_t rawSpeed = static_cast<uint16_t>(speed * 10.0f);
    uint8_t dataSpeed[8] = {0};
    dataSpeed[0] = (rawSpeed >> 8) & 0xFF; dataSpeed[1] = rawSpeed & 0xFF;

    uint16_t rawRpm = static_cast<uint16_t>(rpm);
    uint8_t dataRpm[8] = {0};
    dataRpm[0] = (rawRpm >> 8) & 0xFF; dataRpm[1] = rawRpm & 0xFF;

    uint8_t rawCoolant = static_cast<uint8_t>(coolant + 40.0f); // 带偏移量
    uint8_t dataCoolant[8] = {rawCoolant, 0};

    uint8_t rawBattery = static_cast<uint8_t>(battery / 0.5f);  // 带缩放因子
    uint8_t dataBattery[8] = {rawBattery, 0};

    // 3. 位操作打包开关量
    uint8_t statusByte = 0;
    bool leftLightOn = (qSin(m_angle * 8) > 0);
    bool highBeamOn = true;
    bool seatBeltOn = true;
    if (leftLightOn) statusByte |= (1 << 0);
    if (highBeamOn)  statusByte |= (1 << 1);
    if (seatBeltOn)  statusByte |= (1 << 2);
    uint8_t dataStatus[8] = {statusByte, 0};

    // 4. 拼装协议帧并发送
    m_serial->write(buildFrame(0x100, dataSpeed));
    m_serial->write(buildFrame(0x101, dataRpm));
    m_serial->write(buildFrame(0x102, dataCoolant));
    m_serial->write(buildFrame(0x103, dataBattery));
    m_serial->write(buildFrame(0x200, dataStatus));

    // 5. 限流日志 (20Hz 发送, 1Hz 打印)
    if (++m_sendLogCounter >= 20) {
        m_sendLogCounter = 0;
        qDebug() << QString("发送数据帧: 车速 %1 | 转速 %2 | 水温 %3 | 电量 %4")
                        .arg(speed, 0, 'f', 1).arg(rpm, 0, 'f', 0)
                        .arg(coolant, 0, 'f', 1).arg(battery, 0, 'f', 1);
    }
}

// 提取公共帧组装逻辑 (DRY 原则)
QByteArray Simulator::buildFrame(uint32_t canId, const uint8_t* data) {
    QByteArray frame;
    frame.append(static_cast<char>(0xAA)); // 帧头
    frame.append(static_cast<char>(0x55));
    frame.append(static_cast<char>((canId >> 24) & 0xFF)); // 大端序 CAN ID
    frame.append(static_cast<char>((canId >> 16) & 0xFF));
    frame.append(static_cast<char>((canId >> 8) & 0xFF));
    frame.append(static_cast<char>(canId & 0xFF));
    frame.append(static_cast<char>(8)); // 数据长度
    frame.append(reinterpret_cast<const char*>(data), 8); // 数据域
    return frame;
}