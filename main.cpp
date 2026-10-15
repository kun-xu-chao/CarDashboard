// ============================================================================
// 文件名: main.cpp
// 功能描述: 程序入口，负责搭建三层架构与多线程模型。
// ============================================================================

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <QQuickStyle>
#include "simulator.h"
#include "comm_worker.h"
#include "dashboard_model.h"
#include "mqtt_provider.h"
#include "http_weather_provider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 强制使用 Basic 样式，绕过 Windows 原生样式限制，确保 QML 自定义样式生效
    QQuickStyle::setStyle("Basic");

    // 注册自定义类型，支持跨线程 QueuedConnection 的信号槽传递
    qRegisterMetaType<VehicleData>("VehicleData");

    // ---- 1. 主线程对象 (UI 与数据模型) ----
    Simulator simulator;
    DashboardModel dashboardModel;

    // ---- 2. 子线程对象 (重 I/O 模块) ----
    QThread *commThread = new QThread;
    QThread *mqttThread = new QThread;
    QThread *weatherThread = new QThread;

    CommWorker *worker = new CommWorker();
    MqttProvider *mqttProvider = new MqttProvider();
    HttpWeatherProvider *weatherProvider = new HttpWeatherProvider();

    // 改变对象线程亲和性，避免串口与网络 I/O 阻塞主线程 UI
    worker->moveToThread(commThread);
    mqttProvider->moveToThread(mqttThread);
    weatherProvider->moveToThread(weatherThread);

    // ---- 3. 启动子线程 ----
    QObject::connect(commThread, &QThread::started, worker, &CommWorker::startReceiving);
    QObject::connect(mqttThread, &QThread::started, mqttProvider, &MqttProvider::start);
    QObject::connect(weatherThread, &QThread::started, weatherProvider, &HttpWeatherProvider::start);

    commThread->start();
    mqttThread->start();
    weatherThread->start();

    // ---- 4. 跨线程信号槽连接 (Qt 自动使用 QueuedConnection) ----
    // 串口数据流
    QObject::connect(worker, &CommWorker::dataParsed, &dashboardModel, &DashboardModel::updateData);
    QObject::connect(worker, &CommWorker::logMessage, &dashboardModel, &DashboardModel::logMessage);

    // MQTT 数据流 (局部更新信号, 防止覆盖串口数据)
    QObject::connect(mqttProvider, &MqttProvider::speedUpdated, &dashboardModel, &DashboardModel::updateSpeed);
    QObject::connect(mqttProvider, &MqttProvider::otaProgressUpdated, &dashboardModel, &DashboardModel::updateOtaProgress);
    QObject::connect(mqttProvider, &MqttProvider::logMessage, &dashboardModel, &DashboardModel::logMessage);

    // HTTP 数据流
    QObject::connect(weatherProvider, &HttpWeatherProvider::weatherUpdated, &dashboardModel, &DashboardModel::updateWeather);
    QObject::connect(weatherProvider, &HttpWeatherProvider::logMessage, &dashboardModel, &DashboardModel::logMessage);

    // ---- 5. 将 C++ 对象注入 QML (仅注入主线程对象) ----
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("simulator", &simulator);
    engine.rootContext()->setContextProperty("dashboardModel", &dashboardModel);

    // ---- 6. 加载 QML 界面 ----
    const QUrl url(QStringLiteral("qrc:/qt/qml/CarDashboard/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl) QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);
    return app.exec();
}