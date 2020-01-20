import QtQuick 2.0
import QtQuick.Controls 1.1
import elmo.ClStreamView 1.0

ApplicationWindow{
    id: applicationWindow1
    width: 800
    minimumWidth: 640
    height: 600
    minimumHeight: 480
    color: "transparent"
    title: qsTr("QtSample")
    visible: true

    Rectangle {
        id: rect2
        anchors.fill: parent
        color: "transparent"
        ClStreamView {
            id: stream
            anchors.fill: parent
        }
    }
    Rectangle {
        y: 80
        width: 180; height: 150
        anchors.horizontalCenterOffset: 2
        anchors.horizontalCenter: parent.horizontalCenter
        color: "lime"
        radius: 5
        Label {
            x: 10; y: 10
            id: label
            font.pixelSize: 18
            text: qsTr("Menu")
        }
        Button {
            id: button
            x:30; y:40
            width: 120
            text: "Button"
            onClicked:{
                stream.slotStreamStart("rtsp://192.168.11.2/stream")
                stream.update()
            }
        }
    }
}
