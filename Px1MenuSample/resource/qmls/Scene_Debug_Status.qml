import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

Rectangle {
    id: scene_debug_status
    width: 800
    height: 480
    color: "#ffffffff" //白

    //背景
    Image {
        id: image_background
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "images/PXOSD_R0_Background1.png"
    }

    //タイトル
    Text {
        text: "Status"
        x: 30
        y: 10
        font.pointSize: 24
        font.bold: true
        color: "white"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentDebugStatus }
}
