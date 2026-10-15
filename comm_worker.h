// ============================================================================
// 文件名: comm_worker.h
// 功能描述: 串口通信工作者（接收端，运行于子线程）。
// ============================================================================

#pragma once
#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include "vehicle_data.h"

class CommWorker : public QObject {
    Q_OBJECT
public:
    explicit CommWorker(QObject *parent = nullptr);
    void startReceiving();

signals:
    void dataParsed(const VehicleData& data);
    void logMessage(const QString& msg);

private slots:
    void onReadyRead();

private:
    void parseFrame(const QByteArray& frame);

    QSerialPort *m_serial;
    QByteArray   m_buffer;        // 粘包/断包处理缓冲区
    VehicleData  m_currentData;   // 全局状态快照 (每帧只更新对应字段)
    int          m_recvLogCounter = 0;
};