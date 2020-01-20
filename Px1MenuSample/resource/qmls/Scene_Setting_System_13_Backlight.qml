import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_system_backlight
    width: 800
    height: 480

    function setNormal()
    {
        button_setting3_backlight_off.setNormal()
        button_setting3_backlight_30s.setNormal()
        button_setting3_backlight_01m.setNormal()
        button_setting3_backlight_05m.setNormal()
        button_setting3_backlight_10m.setNormal()
    }

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
        source: "qrc:///Set3_S2_Backlight_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    //設定ボタン
    Button_Setting3 { id: button_setting3_backlight_off; x: 50; y: 168; filePath: "image://image_provider/:/Set3_S2_Backlight_1"; strCmd: "Backlight"; strParam: "OFF"; stateButton: ScreenMsgProc.state_set2_S1_13 == "OFF" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_backlight_30s; x: 420; y: 168; filePath: "image://image_provider/:/Set3_S2_Backlight_2"; strCmd: "Backlight"; strParam: "30s"; stateButton: ScreenMsgProc.state_set2_S1_13 == "30s" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_backlight_01m; x: 50; y: 242; filePath: "image://image_provider/:/Set3_S2_Backlight_3"; strCmd: "Backlight"; strParam: "01m"; stateButton: ScreenMsgProc.state_set2_S1_13 == "01m" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_backlight_05m; x: 420; y: 242; filePath: "image://image_provider/:/Set3_S2_Backlight_4"; strCmd: "Backlight"; strParam: "05m"; stateButton: ScreenMsgProc.state_set2_S1_13 == "05m" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_backlight_10m; x: 50; y: 316; filePath: "image://image_provider/:/Set3_S2_Backlight_5"; strCmd: "Backlight"; strParam: "10m"; stateButton: ScreenMsgProc.state_set2_S1_13 == "10m" ? "S" : "N" }
}
