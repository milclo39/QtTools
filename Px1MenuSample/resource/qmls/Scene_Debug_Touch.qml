import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

Rectangle {
    id: scene_debug_touch
    width: 800
    height: 480
    color: "#ffffffff" //白

    Rectangle {
        id: ellipse
        width: 10
        height: width
        x: 0
        y: 0
        color: define.colorBar
        border.color:  define.colorBar
        border.width: 1
        radius: width*0.5
        visible: false
    }

    //座標確認用
    MouseArea {
        anchors.fill: parent
        onClicked: {
            textPosition.text = "[Touch Panel]  Event( Clicked ) \t Position( " + mouseX + " , " + mouseY + " )"
            ellipse.visible = true
            ellipse.x = mouseX - 5
            ellipse.y = mouseY - 5
        }
        onMouseXChanged: {
            textPosition.text = "[Touch Panel]  Event( Move ) \t Position( " + mouseX + " , " + mouseY + " )"
            ellipse.x = mouseX - 5
            ellipse.y = mouseY - 5
        }
    }
    Text {
        id: textPosition
        text: "[Touch Panel]  Event( - ) \t Position( - , - )"
        x: 5
        y: 455
        font.pointSize: 9
        font.bold: true
        color: "black"
    }
}
