import QtQuick 2.10
import QtQuick.Window 2.10
import elmo.ClStreamView 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("libavtest")
    // libavの描画領域
    Rectangle {
        id: streamview
        anchors.fill: parent
        visible: true
        color: "transparent"
        ClStreamView {
            id: stream
            anchors.fill: parent
        }
    }
    Rectangle{
        id: btn_select
        x:0; y: 0; height: 24; width: 48
        color:"lightgreen"
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:"Select"
        }
        MouseArea {
            anchors.fill: parent
            onPressed: {
                stream.selectCam()
            }
        }
    }
    Rectangle{
        id: btn_start
        x:50; y: 0; height: 24; width: 48
        color:"lightgreen"
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:"Start"
        }
        MouseArea {
            anchors.fill: parent
            onPressed: {
                stream.startLive()
            }
        }
    }
    Rectangle{
        id: btn_stop
        x:100; y: 0; height: 24; width: 48
        color:"lightgreen"
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:"Stop"
        }
        MouseArea {
            anchors.fill: parent
            onPressed: {
                stream.stopLive()
            }
        }
    }
    Rectangle{
        id: btn_execaf
        x:150; y: 0; height: 24; width: 48
        color:"pink"
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:"AF"
        }
        MouseArea {
            anchors.fill: parent
            onPressed: {
                stream.execAf()
            }
        }
    }
    Rectangle{
        id: btn_zoomtele
        x:200; y: 0; height: 24; width: 48
        color:"lightblue"
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:"TELE"
        }
        MouseArea {
            anchors.fill: parent
            onPressed: {
                stream.zoomTele()
            }
            onReleased: {
                stream.stopCtrl()
            }
        }
    }
    Rectangle{
        id: btn_zoomwide
        x:250; y: 0; height: 24; width: 48
        color:"lightblue"
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:"WIDE"
        }
        MouseArea {
            anchors.fill: parent
            onPressed: {
                stream.zoomWide()
            }
            onReleased: {
                stream.stopCtrl()
            }
        }
    }
    Rectangle{
        id: btn_irisopen
        x:300; y: 0; height: 24; width: 48
        color:"lightblue"
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:"BRIGHT"
        }
        MouseArea {
            anchors.fill: parent
            onPressed: {
                stream.irisOpen()
            }
            onReleased: {
                stream.stopCtrl()
            }
        }
    }
    Rectangle{
        id: btn_irisclose
        x:350; y: 0; height: 24; width: 48
        color:"lightblue"
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text:"DARK"
        }
        MouseArea {
            anchors.fill: parent
            onPressed: {
                stream.irisClose()
            }
            onReleased: {
                stream.stopCtrl()
            }
        }
    }
}
