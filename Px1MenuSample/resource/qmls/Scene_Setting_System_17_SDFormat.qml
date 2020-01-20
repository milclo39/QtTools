import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2

Rectangle {
    id: scene_setting_system_sdformat
    width: 800
    height: 480

    property bool visibleButton: true

    //背景
    Image {
        id: image_background
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "images/PXOSD_R0_Background1.png"
    }

    //タイトル
    Image {
        id: image_title
        x: 0
        y: 0
        width: 800
        height: 60
        source: "qrc:///Set3_S2_SDformat_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem; visible: visibleButton }

    //設定ボタン
    Button_Setting3 { id: button_setting3_sdformat_yes; x: 50; y: 242; filePath: "image://image_provider/:/Set3_S2_Initializ_Yes"; strCmd: "SDFormat"; strParam: "Yes"; visible: visibleButton }
    Button_Setting3 { id: button_setting3_sdformat_no; x: 420; y: 242; filePath: "image://image_provider/:/Set3_S2_Initializ_No"; strCmd: "SDFormat"; strParam: "No"; visible: visibleButton }
}
