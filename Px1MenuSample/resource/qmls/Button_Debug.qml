import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

//デバッグボタン
Rectangle {
    id: button_debug
    width: 680
    height: 50

    property string text: "Debug"
    property Component component: componentDebug

    Image {
        id: image_button_debug
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "image://image_provider/:/qmls/images/blank_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_button_debug.source = "image://image_provider/:/qmls/images/blank_P.png"
            }
            onContainsMouseChanged: {
                image_button_debug.source = "image://image_provider/:/qmls/images/blank_N.png"
            }
            onCanceled: {
                image_button_debug.source = "image://image_provider/:/qmls/images/blank_N.png"
            }
            onReleased: {
                if (image_button_debug.source == "image://image_provider/:/qmls/images/blank_P.png") {
                    if (component == componentDebugTouch) {
                        text_debug.visible = false
                    }
                    if (text == "Grid") {
                        text_debug.y = 10
                    } else {
                        text_debug.y = 70
                    }
                    stackView.push(component)
                }
            }
        }
    }
    Text {
        anchors.centerIn: parent
        text: button_debug.text
        font.pointSize: 18
        color: "grey"
    }
}
