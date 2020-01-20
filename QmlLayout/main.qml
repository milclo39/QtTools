import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.3
import elmo.Worker 1.0
import "screens" as Screens

ApplicationWindow {
    visible: true
    title: qsTr("QmlLayout Test")
    width: 800
    height: 480

    Rectangle {
        id: main
        implicitWidth: parent.width
        implicitHeight: parent.height
        anchors.fill: parent

        Loader { id: loader}

        Component.onCompleted: {
            loader.source = "screens/Layouts.qml"
        }
    }
}
