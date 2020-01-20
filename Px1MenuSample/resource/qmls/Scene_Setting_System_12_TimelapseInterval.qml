import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_system_timepapseinterval
    width: 800
    height: 480

    function setNormal()
    {
        button_setting3_timepapseinterval_01m.setNormal()
        button_setting3_timepapseinterval_10m.setNormal()
        button_setting3_timepapseinterval_30m.setNormal()
        button_setting3_timepapseinterval_01h.setNormal()
        button_setting3_timepapseinterval_03h.setNormal()
        button_setting3_timepapseinterval_06h.setNormal()
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
        source: "qrc:///Set3_S2_TimelapseInterval_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    //設定ボタン
    Button_Setting3 { id: button_setting3_timepapseinterval_01m; x: 50; y: 168; filePath: "image://image_provider/:/Set3_S2_TimelapseInterval_1"; strCmd: "TimelapseInterval"; strParam: "01m"; stateButton: ScreenMsgProc.state_set2_S1_12 == "01m" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_timepapseinterval_10m; x: 420; y: 168; filePath: "image://image_provider/:/Set3_S2_TimelapseInterval_2"; strCmd: "TimelapseInterval"; strParam: "10m"; stateButton: ScreenMsgProc.state_set2_S1_12 == "10m" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_timepapseinterval_30m; x: 50; y: 242; filePath: "image://image_provider/:/Set3_S2_TimelapseInterval_3"; strCmd: "TimelapseInterval"; strParam: "30m"; stateButton: ScreenMsgProc.state_set2_S1_12 == "30m" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_timepapseinterval_01h; x: 420; y: 242; filePath: "image://image_provider/:/Set3_S2_TimelapseInterval_4"; strCmd: "TimelapseInterval"; strParam: "01h"; stateButton: ScreenMsgProc.state_set2_S1_12 == "01h" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_timepapseinterval_03h; x: 50; y: 316; filePath: "image://image_provider/:/Set3_S2_TimelapseInterval_5"; strCmd: "TimelapseInterval"; strParam: "03h"; stateButton: ScreenMsgProc.state_set2_S1_12 == "03h" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_timepapseinterval_06h; x: 420; y: 316; filePath: "image://image_provider/:/Set3_S2_TimelapseInterval_6"; strCmd: "TimelapseInterval"; strParam: "06h"; stateButton: ScreenMsgProc.state_set2_S1_12 == "06h" ? "S" : "N" }
}
