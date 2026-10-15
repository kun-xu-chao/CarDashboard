// ============================================================================
// 文件名: Main.qml
// 功能描述: 汽车仪表盘调试面板 QML 界面。
// ============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    width: 950
    height: 750
    visible: true
    title: "汽车仪表盘调试面板"

    // 设计令牌 (Design Tokens): 集中定义全局颜色，便于主题切换
    color: "#0A0E14"
    palette.windowText: "#C9D1D9"

    property color cardBgColor: "#161B22"
    property color cardBorderColor: "#30363D"
    property color textTitleColor: "#8B949E"
    property color textValueColor: "#C9D1D9"
    property color speedColor: "#00E5FF"
    property color rpmColor: "#FF9F1C"
    property color safeColor: "#3FB950"
    property color warnColor: "#F85149"
    property color logSendColor: "#8B949E"
    property color logRecvColor: "#58A6FF"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // ============ 1. 模拟器控制 ============
        GroupBox {
            title: "模拟器控制 (发送端 COM1)"
            Layout.fillWidth: true
            padding: 15
            topPadding: 40

            background: Rectangle { color: cardBgColor; radius: 12; border.color: cardBorderColor }
            label: Text { text: parent.title; color: textTitleColor; font.pixelSize: 16; font.bold: true }

            RowLayout {
                spacing: 15
                Button {
                    text: "开始模拟"
                    hoverEnabled: true
                    onClicked: simulator.start()
                    background: Rectangle {
                        color: parent.pressed ? "#1F6FEB" : (parent.hovered ? "#388BFD" : "#238636")
                        radius: 8; border.color: parent.hovered ? "#58A6FF" : "transparent"; border.width: 1
                    }
                    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                }
                Button {
                    text: "停止模拟"
                    hoverEnabled: true
                    onClicked: simulator.stop()
                    background: Rectangle {
                        color: parent.pressed ? "#DA3633" : (parent.hovered ? "#F85149" : "#B62324")
                        radius: 8; border.color: parent.hovered ? "#FF7B72" : "transparent"; border.width: 1
                    }
                    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                }
                Text { text: "状态：点击按钮启停"; color: textTitleColor; font.pixelSize: 14 }
            }
        }

        // ============ 2. 仪表盘数据 ============
        GroupBox {
            title: "仪表盘数据 (接收端 COM2)"
            Layout.fillWidth: true
            padding: 15
            topPadding: 40

            background: Rectangle { color: cardBgColor; radius: 12; border.color: cardBorderColor }
            label: Text { text: parent.title; color: textTitleColor; font.pixelSize: 16; font.bold: true }

            ColumnLayout {
                spacing: 20
                Layout.alignment: Qt.AlignHCenter

                RowLayout {
                    spacing: 100
                    ColumnLayout {
                        Text { text: "车速"; font.pixelSize: 20; color: textTitleColor; Layout.alignment: Qt.AlignHCenter }
                        Text { text: dashboardModel.speed.toFixed(1); font.pixelSize: 90; font.bold: true; color: speedColor }
                        Text { text: "km/h"; font.pixelSize: 16; color: textTitleColor; Layout.alignment: Qt.AlignHCenter }
                    }
                    ColumnLayout {
                        Text { text: "转速"; font.pixelSize: 20; color: textTitleColor; Layout.alignment: Qt.AlignHCenter }
                        Text { text: dashboardModel.rpm.toFixed(0); font.pixelSize: 90; font.bold: true; color: rpmColor }
                        Text { text: "rpm"; font.pixelSize: 16; color: textTitleColor; Layout.alignment: Qt.AlignHCenter }
                    }
                }

                RowLayout {
                    spacing: 50
                    ColumnLayout {
                        Text { text: "水温 " + dashboardModel.coolantTemp.toFixed(0) + " ℃"; color: textValueColor; font.pixelSize: 16 }
                        Rectangle {
                            width: 180; height: 12; radius: 6; color: "#21262D"
                            Rectangle {
                                width: parent.width * Math.max(0, Math.min(1, (dashboardModel.coolantTemp + 40) / 160))
                                height: parent.height; radius: 6
                                color: dashboardModel.coolantTemp > 90 ? warnColor : "#00B4D8"
                                Behavior on width { NumberAnimation { duration: 200 } }
                            }
                        }
                    }
                    ColumnLayout {
                        Text { text: "电量 " + dashboardModel.batterySoc.toFixed(0) + " %"; color: textValueColor; font.pixelSize: 16 }
                        Rectangle {
                            width: 180; height: 12; radius: 6; color: "#21262D"
                            Rectangle {
                                width: parent.width * Math.max(0, Math.min(1, dashboardModel.batterySoc / 100))
                                height: parent.height; radius: 6
                                color: dashboardModel.batterySoc < 20 ? warnColor : safeColor
                                Behavior on width { NumberAnimation { duration: 200 } }
                            }
                        }
                    }
                    ColumnLayout {
                        spacing: 10
                        Text { text: "🌡️"; font.pixelSize: 18 }
                        Text { text: dashboardModel.weatherTemp + " " + dashboardModel.weatherDesc; color: "#00E5FF"; font.pixelSize: 16; font.bold: true }
                    }
                }
            }
        }

        // ============ 3. 车辆状态 ============
        GroupBox {
            title: "车辆状态"
            Layout.fillWidth: true
            padding: 15
            topPadding: 40

            background: Rectangle { color: cardBgColor; radius: 12; border.color: cardBorderColor }
            label: Text { text: parent.title; color: textTitleColor; font.pixelSize: 16; font.bold: true }

            RowLayout {
                spacing: 40
                RowLayout {
                    spacing: 10
                    Rectangle { width: 24; height: 24; radius: 12; color: dashboardModel.leftTurnLight ? safeColor : "#21262D"; border.color: dashboardModel.leftTurnLight ? "#56D364" : cardBorderColor; border.width: 2; Behavior on color { ColorAnimation { duration: 150 } } }
                    Text { text: "左转"; color: textValueColor; font.pixelSize: 18 }
                }
                RowLayout {
                    spacing: 10
                    Rectangle { width: 24; height: 24; radius: 12; color: dashboardModel.highBeam ? "#388BFD" : "#21262D"; border.color: dashboardModel.highBeam ? "#58A6FF" : cardBorderColor; border.width: 2; Behavior on color { ColorAnimation { duration: 150 } } }
                    Text { text: "远光"; color: textValueColor; font.pixelSize: 18 }
                }
                RowLayout {
                    spacing: 10
                    Rectangle { width: 24; height: 24; radius: 12; color: dashboardModel.seatBelt ? safeColor : warnColor; border.color: dashboardModel.seatBelt ? "#56D364" : "#FF7B72"; border.width: 2; Behavior on color { ColorAnimation { duration: 150 } } }
                    Text { text: "安全带"; color: textValueColor; font.pixelSize: 18 }
                }
            }
        }

        // ============ 4. 日志区 ============
        GroupBox {
            title: "接收解析日志"
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 15
            topPadding: 40

            background: Rectangle { color: cardBgColor; radius: 12; border.color: cardBorderColor }
            label: Text { text: parent.title; color: textTitleColor; font.pixelSize: 16; font.bold: true }

            ScrollView {
                id: logScroll
                anchors.fill: parent
                clip: true

                ScrollBar.vertical: ScrollBar {
                    id: vbar
                    width: 8
                    policy: ScrollBar.AlwaysOn
                    background: Rectangle { color: "#161B22"; radius: 4 }
                    contentItem: Rectangle { color: vbar.pressed ? "#58A6FF" : "#30363D"; radius: 4 }
                }

                background: Rectangle { color: "#010409"; radius: 8; border.color: "#21262D"; border.width: 1 }

                TextArea {
                    id: logArea
                    readOnly: true
                    text: "等待数据...\n"
                    color: textValueColor
                    font.family: "Consolas"
                    font.pixelSize: 14
                    wrapMode: TextArea.Wrap
                    textFormat: TextEdit.RichText
                    topPadding: 10; leftPadding: 15; rightPadding: 15; bottomPadding: 10

                    onTextChanged: {
                        var maxLines = 300
                        var lines = text.split("\n")
                        if (lines.length > maxLines) {
                            text = lines.slice(lines.length - maxLines).join("\n")
                        }
                        cursorPosition = text.length

                        // 延迟到下一帧布局完成后再滚动，确保 contentHeight 计算正确
                        Qt.callLater(function() {
                            if (logScroll.contentItem) {
                                logScroll.contentItem.contentY = logScroll.contentItem.contentHeight - logScroll.height
                            }
                        })
                    }
                }
            }
        }
    }

    // ============ OTA 升级弹窗 ============
    Popup {
        id: otaPopup
        width: 400; height: 200
        anchors.centerIn: parent
        modal: true; focus: true
        closePolicy: Popup.NoAutoClose

        background: Rectangle { color: "#161B22"; radius: 16; border.color: "#30363D"; border.width: 2 }

        ColumnLayout {
            anchors.fill: parent; anchors.margins: 20; spacing: 15
            Text { text: dashboardModel.otaProgress < 100 ? "OTA 升级中..." : "OTA 升级完成"; color: "#C9D1D9"; font.pixelSize: 20; font.bold: true; Layout.alignment: Qt.AlignHCenter }
            ProgressBar {
                id: otaProgressBar
                Layout.fillWidth: true; from: 0; to: 100; value: dashboardModel.otaProgress
                background: Rectangle { color: "#21262D"; radius: 6; implicitHeight: 12 }
                contentItem: Item {
                    Rectangle {
                        width: otaProgressBar.visualPosition * parent.width; height: parent.height; radius: 6
                        color: dashboardModel.otaProgress < 100 ? "#00E5FF" : "#3FB950"
                        Behavior on width { NumberAnimation { duration: 200 } }
                    }
                }
            }
            Text { text: dashboardModel.otaProgress + " %"; color: "#8B949E"; font.pixelSize: 18; Layout.alignment: Qt.AlignHCenter }
        }
        // 注意：绝不能在这里写 onVisibleChanged，否则会导致关闭后立刻又打开的循环
    }

    Connections {
        target: dashboardModel
        function onOtaProgressChanged() {
            if (dashboardModel.otaProgress > 0 && dashboardModel.otaProgress < 100) {
                if (!otaPopup.visible) otaPopup.open()
            }
            if (dashboardModel.otaProgress >= 100) {
                if (!closeTimer.running) closeTimer.start()
            }
        }
    }

    Timer {
        id: closeTimer
        interval: 3000
        onTriggered: otaPopup.close()
    }

    // ============ 日志信号连接 ============
    Connections {
        target: simulator
        function onLogMessage(msg) { logArea.append("<font color='" + logSendColor + "'>[发送] " + msg + "</font>") }
    }

    Connections {
        target: dashboardModel
        function onLogMessage(msg) { logArea.append("<font color='" + logRecvColor + "'>[接收] " + msg + "</font>") }
    }
}