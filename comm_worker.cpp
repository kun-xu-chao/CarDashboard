// ============================================================================
// 文件名: comm_worker.cpp
// 功能描述: 串口通信工作者具体实现。处理粘包、解析 CAN 帧、DBC 解码。
// ============================================================================

#include "comm_worker.h"

CommWorker::CommWorker(QObject *parent) : QObject(parent) {
    m_serial = new QSerialPort(this);
    m_serial->setPortName("COM2");
    m_serial->setBaudRate(QSerialPort::Baud115200);
    connect(m_serial, &QSerialPort::readyRead, this, &CommWorker::onReadyRead);
}

void CommWorker::startReceiving() {
    if (m_serial->open(QIODevice::ReadOnly)) {
        m_serial->clear();
        m_buffer.clear();
        emit logMessage("成功打开 COM2，开始监听数据...");
    } else {
        emit logMessage("错误：无法打开 COM2！");
    }
}

void CommWorker::onReadyRead() {
    m_buffer.append(m_serial->readAll());

    // 粘包/断包处理：循环寻找帧头 0xAA 0x55，切割定长 15 字节帧
    while (m_buffer.size() >= 15) {
        if (static_cast<uint8_t>(m_buffer[0]) != 0xAA ||
            static_cast<uint8_t>(m_buffer[1]) != 0x55) {
            m_buffer.remove(0, 1); // 错位恢复：丢弃 1 字节重新对齐
            continue;
        }

        QByteArray frame = m_buffer.left(15);
        m_buffer.remove(0, 15);
        parseFrame(frame);
    }
}

void CommWorker::parseFrame(const QByteArray& frame) {
    // 1. 提取 CAN ID (大端序)
    uint32_t canId = (static_cast<uint8_t>(frame[2]) << 24) |
                     (static_cast<uint8_t>(frame[3]) << 16) |
                     (static_cast<uint8_t>(frame[4]) << 8)  |
                     static_cast<uint8_t>(frame[5]);

    // 2. 提取 8 字节数据域
    uint8_t data[8];
    for (int i = 0; i < 8; ++i) {
        data[i] = static_cast<uint8_t>(frame[7 + i]);
    }

    // 3. DBC 解码与位操作还原 (更新全局快照对应字段)
    if (canId == 0x100) {
        uint16_t rawSpeed = (data[0] << 8) | data[1];
        m_currentData.speed = rawSpeed * 0.1f;
    }
    else if (canId == 0x101) {
        uint16_t rawRpm = (data[0] << 8) | data[1];
        m_currentData.rpm = rawRpm * 1.0f;
    }
    else if (canId == 0x102) {
        uint8_t rawCoolant = data[0];
        m_currentData.coolantTemp = static_cast<float>(rawCoolant) - 40.0f;
    }
    else if (canId == 0x103) {
        uint8_t rawBattery = data[0];
        m_currentData.batterySoc = static_cast<float>(rawBattery) * 0.5f;
    }
    else if (canId == 0x200) {
        uint8_t status = data[0];
        m_currentData.leftTurnLight = (status & (1 << 0)) != 0;
        m_currentData.highBeam      = (status & (1 << 1)) != 0;
        m_currentData.seatBelt      = (status & (1 << 2)) != 0;
    }

    // 4. 发送全量快照给主线程 (避免分帧到达导致 UI 瞬态缺字段)
    emit dataParsed(m_currentData);

    // 5. 接收端日志限流 (每 100 帧发一次心跳)
    if (++m_recvLogCounter >= 100) {
        m_recvLogCounter = 0;
        emit logMessage(QString("数据流转正常: 车速%1 | 转速%2 | 水温%3 | 电量%4")
                            .arg(m_currentData.speed, 0, 'f', 1)
                            .arg(m_currentData.rpm, 0, 'f', 0)
                            .arg(m_currentData.coolantTemp, 0, 'f', 0)
                            .arg(m_currentData.batterySoc, 0, 'f', 0));
    }
}