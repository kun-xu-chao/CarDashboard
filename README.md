# 🚗 基于 Qt6/QML 的智能座舱汽车仪表盘系统

[![Language](https://img.shields.io/badge/Language-C++17-blue.svg)]()
[![Framework](https://img.shields.io/badge/Framework-Qt6-green.svg)]()
[![Build](https://img.shields.io/badge/Build-CMake-orange.svg)]()

## 📖 项目简介
本项目在无真实硬件环境下，使用虚拟串口（VSPD）模拟真实 CAN 总线数据流，构建了一套具备车云通信、暗黑主题及高流畅动画的现代汽车仪表盘。项目采用 **MVVM 架构** 与 **多线程模型**，完整还原了从数据编码、底层串口传输、粘包解析到 UI 渲染的全链路。

## 🏗️ 系统架构
```text
[Simulator 模拟器] (生成物理值 -> DBC编码 -> 打包CAN帧)
       │ 写 COM1
       ▼
[VSPD 虚拟串口桥接]
       │ 读 COM2
       ▼
[CommWorker 接收端] (子线程：粘包处理、CAN解析、位操作解码)
       │ emit dataParsed (QueuedConnection)
       ▼
[DashboardModel 数据模型] (主线程：Diff-then-Emit 按需刷新)
       │ Q_PROPERTY 属性绑定
       ▼
[Main.qml 界面层] (动态渲染车速、转速、水温、电量、灯光、日志)
