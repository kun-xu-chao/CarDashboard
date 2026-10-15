# ============================================================================
# 文件名: CMakeLists.txt
# 功能描述: Qt6 汽车仪表盘项目构建脚本。
# ============================================================================

cmake_minimum_required(VERSION 3.16)
project(CarDashboard VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找所需的 Qt6 模块 (Mqtt 为手动编译安装)
find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 SerialPort Network Mqtt)

# Qt6 标准项目设置: 开启 AUTOMOC/AUTORCC, 设定 C++17 基线
qt_standard_project_setup(REQUIRES 6.10)

# 添加可执行目标 (按功能模块分组源文件)
qt_add_executable(appCarDashboard
    main.cpp

    vehicle_data.h          # 数据基础
    idata_provider.h

    simulator.h             # 串口通信
    simulator.cpp
    comm_worker.h
    comm_worker.cpp

    dashboard_model.h       # 数据模型
    dashboard_model.cpp

    mqtt_provider.h         # 车云网联
    mqtt_provider.cpp
    http_weather_provider.h
    http_weather_provider.cpp
)

# 添加 QML 模块
qt_add_qml_module(appCarDashboard
    URI CarDashboard
    QML_FILES
        Main.qml
)

# 平台属性设定 (Windows 下不弹黑框, macOS 生成 .app)
set_target_properties(appCarDashboard PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 链接 Qt 库 (PRIVATE 表示这些库仅为当前目标的实现细节, 不对外暴露)
target_link_libraries(appCarDashboard PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::SerialPort
    Qt6::Network
    Qt6::Mqtt
)

install(TARGETS appCarDashboard
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)