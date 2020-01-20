import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

Rectangle {
    id: button_back
    x: 740
    y: 0
    width: 60
    height: 60
    color: "#00000000" //透明

    property string filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back1"
    property Component component: componentSetting

    Image {
        id: image_button_back
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: button_back.filePath + "N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                //console.log("onPressed >> Back")
                if (msg_08.visible == false && msg_09.visible == false) {
                    image_button_back.source = button_back.filePath + "P.png"
                }
            }
            onContainsMouseChanged: {
                //console.log("onContainsMouseChanged >> Back")
                if (msg_08.visible == false && msg_09.visible == false) {
                    image_button_back.source = button_back.filePath + "N.png"
                }
            }
            onReleased: {
                //console.log("Released >> Back")
                if (msg_08.visible == false && msg_09.visible == false) {
                    root.clearMessage() //メッセージがあれば非表示にする
                    if (image_button_back.source == button_back.filePath + "P.png") {
                        if (button_back.filePath == "image://image_provider/:/qmls/images/PXOSD_R0_Back1") { //閉じる場合のみ処理
                            text_debug.y = 10
                        } else {
                            text_debug.y = 70
                        }
                        if (component == componentInputSelectMediaSDImage ||
                            component == componentInputSelectMediaSDVideo ||
                            component == componentInputSelectMediaUSBImage ) {
                            text_debug.y = 10
                        }
                        if (component == componentInputSelectMediaSDImagePlayer ||
                            component == componentInputSelectMediaSDVideoPlayer ||
                            component == componentInputSelectMediaUSBImagePlayer ) {
                            text_debug.y = 10
                        }
                        image_button_back.source = button_back.filePath + "N.png"
                        stackView.pop(component)
                    }
                }
            }
        }
    }
}
