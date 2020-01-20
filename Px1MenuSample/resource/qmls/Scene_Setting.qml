import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

Rectangle {
    id: scene_setting
    width: 800
    height: 480

    property bool visibleDebug: false

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
        source: "qrc:///Set1_title.png"
    }

    //閉じるボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back1"; component: componentSetting }

    //ボタンリスト
    Rectangle {
        x: 30
        y: 94
        width: 750
        height: (50 + 24) * 5

        Grid {
            id: grid_button
            columns: 1
            anchors.centerIn: parent
            spacing: 24

            Button_Setting1{ filePath: "image://image_provider/:/Set1_1"; component: componentSettingSystem } //システム設定ボタン
            Button_Setting1{ filePath: "image://image_provider/:/Set1_2"; component: componentSettingCamera } //カメラ設定ボタン
            Button_Setting1{ filePath: "image://image_provider/:/Set1_3"; component: componentSettingNetwork } //ネットワーク設定ボタン
            Button_Setting1{ filePath: "image://image_provider/:/Set1_4"; component: componentSettingProductInfo } //製品情報ボタン
            Button_Debug{ width:400; text: "Debug"; component: componentDebug; visible: scene_setting.visibleDebug } //Debugボタン
        }
    }
}
