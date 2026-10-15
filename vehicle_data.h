// ============================================================================
// 文件名: vehicle_data.h
// 功能描述: 定义统一车辆数据结构体 VehicleData。
// ============================================================================

#pragma once
#include <QMetaType>
#include <QString>

// 车辆数据快照，聚合分散在多个 CAN 帧中的状态。
// 设计为 POD 风格并内置默认值，支持跨线程深拷贝传递。
struct VehicleData {
    // 连续物理量
    float speed = 0.0f;          // 车速 km/h
    float rpm = 0.0f;            // 转速 rpm
    float coolantTemp = 0.0f;    // 水温 ℃
    float batterySoc = 0.0f;     // 电池电量 %

    // 离散开关量
    bool leftTurnLight = false;  // 左转向灯
    bool highBeam = false;       // 远光灯
    bool seatBelt = false;       // 安全带
};

// 声明为 Qt 元类型，支持跨线程 QueuedConnection 深拷贝。
// 注意：必须在 main.cpp 中调用 qRegisterMetaType<VehicleData>("VehicleData");
Q_DECLARE_METATYPE(VehicleData)