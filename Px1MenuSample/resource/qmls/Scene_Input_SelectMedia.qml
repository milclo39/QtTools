import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

Rectangle {
    id: scene_input_selectmedia
    width: 800
    height: 480
    color: "#00000000" //透明

    //背景
    Image {
        id: image_background
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Background1.png"
    }

    //タイトル
    Image {
        id: image_title
        x: 0
        y: 0
        width: 800
        height: 60
        source: "qrc:///INPUT2_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentInput }

    //設定ボタン
    Button_Setting3 { id: button_input_sd_image; x: 50; y:168; filePath: "image://image_provider/:/INPUT2_1"; strCmd: "SelectMedia"; strParam: "SDimage" }
    Button_Setting3 { id: button_input_sd_vide; x: 420; y:168; filePath: "image://image_provider/:/INPUT2_2"; strCmd: "SelectMedia"; strParam: "SDvideo" }
    Button_Setting3 { id: button_input_usb_image; x: 50; y:242; filePath: "image://image_provider/:/INPUT2_3"; strCmd: "SelectMedia"; strParam: "USBimage" }
}
