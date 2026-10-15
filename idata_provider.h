// ============================================================================
// 文件名: idata_provider.h
// 功能描述: 数据源抽象接口，遵循依赖倒置原则（DIP）。
// ============================================================================

#pragma once
#include <QObject>


class IDataProvider : public QObject {
    Q_OBJECT
public:
    explicit IDataProvider(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IDataProvider() = default;

    // 启动数据源 (Q_INVOKABLE 支持 QML 直接调用)
    Q_INVOKABLE virtual void start() = 0;

    // 停止数据源
    Q_INVOKABLE virtual void stop() = 0;

};