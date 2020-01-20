import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import Network 1.0

ApplicationWindow {
    id: _root
    visible: true
    width: 640
    height: 480
    title: "QmlNetwork"
//    flags:Qt.FramelessWindowHint

    property int pxport: 5136
    property int btnsize: 200

    Network { id: network }
    onClosing: {
        network.setIpaddr(txtIpaddr.text)
        network.setMacaddr(txtMacaddr.text)
        network.close()
    }
    Rectangle {
        id: btnPowerOff
        color: "transparent"
        x: 0;y: 0
        width: btnsize;height: btnsize
        MouseArea {
            anchors.fill: parent
            drag.target: parent
            onClicked: {
                network.connectToHost(txtIpaddr.text, pxport)
                network.write("{\"power\":\"off\"}")
            }
            Image{
                anchors.fill: parent
                source: "qrc:/fl_off.png"
            }
            onHoveredChanged: {parent.opacity = containsMouse? 0.6 : 1}
            onPressed: {parent.width -= 5;parent.height -= 5}
            onReleased: {parent.width += 5;parent.height += 5}
        }
    }
    Rectangle {
        id: btnSwitchCam
        color: "transparent"
        x: btnsize;y: 0
        width: btnsize;height: btnsize
        MouseArea {
            anchors.fill: parent
            drag.target: parent
            onClicked: {
                network.connectToHost(txtIpaddr.text, pxport)
                network.write("{\"switch input\":\"camera\"}")
            }
            Image{
                anchors.fill: parent
                source: "qrc:/ohc.png"
            }
            onHoveredChanged: {parent.opacity = containsMouse? 0.6 : 1}
            onPressed: {parent.width -= 5;parent.height -= 5}
            onReleased: {parent.width += 5;parent.height += 5}
        }
    }
    Rectangle {
        id: btnSwitchHdmi
        color: "transparent"
        x: 0;y: btnsize
        width: btnsize;height: btnsize
        MouseArea {
            anchors.fill: parent
            drag.target: parent
            onClicked: {
                network.connectToHost(txtIpaddr.text, pxport)
                network.write("{\"switch input\":\"hdmi1\"}")
            }
            Image{
                anchors.fill: parent
                source: "qrc:/pc.png"
            }
            onHoveredChanged: {parent.opacity = containsMouse? 0.6 : 1}
            onPressed: {parent.width -= 5;parent.height -= 5}
            onReleased: {parent.width += 5;parent.height += 5}
        }
    }
    Rectangle {
        id: btnPowerOn
        color: "transparent"
        x: btnsize;y: btnsize
        width: btnsize;height: btnsize
        MouseArea {
            anchors.fill: parent
            drag.target: parent
            onClicked: {
                network.wakeOnLan(txtIpaddr.text, txtMacaddr.text)
            }
            Image{
                anchors.fill: parent
                source: "qrc:/fl_on.png"
            }
            onHoveredChanged: {parent.opacity = containsMouse? 0.6 : 1}
            onPressed: {parent.width -= 5;parent.height -= 5}
            onReleased: {parent.width += 5;parent.height += 5}
        }
    }
    // 設定
    Row {
        id: setting
        spacing: btnsize / 12
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        Label{
            anchors.verticalCenter: parent.verticalCenter
            text: "IP Address:"
        }
        TextField {
            id: txtIpaddr
            width: btnsize
            font.pointSize: 16
            placeholderText: qsTr("*.*.*.*")
            selectByMouse: true
            text: network.getIpaddr()
        }
        Label{
            anchors.verticalCenter: parent.verticalCenter
            text: "MAC Address:"
        }
        TextField {
            id: txtMacaddr
            width: btnsize
            font.pointSize: 16
            placeholderText: qsTr("************")
            selectByMouse: true
            text: network.getMacaddr()
        }
    }
}
