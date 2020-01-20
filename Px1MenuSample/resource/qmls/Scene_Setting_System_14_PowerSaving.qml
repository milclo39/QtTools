import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_system_powersaving
    width: 800
    height: 480

    function setNormal()
    {
        button_setting3_powersaving_off.setNormal()
        button_setting3_powersaving_30m.setNormal()
        button_setting3_powersaving_01h.setNormal()
        button_setting3_powersaving_02h.setNormal()
        button_setting3_powersaving_03h.setNormal()
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
        source: "qrc:///Set3_S2_PowerSaving_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    //設定ボタン
    Button_Setting3 { id: button_setting3_powersaving_off; x: 50; y :168; filePath: "image://image_provider/:/Set3_S2_PowerSaving_1"; strCmd: "PowerSaving"; strParam: "OFF"; stateButton: ScreenMsgProc.state_set2_S1_14 == "OFF" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_powersaving_30m; x: 420; y: 168; filePath: "image://image_provider/:/Set3_S2_PowerSaving_2"; strCmd: "PowerSaving"; strParam: "30m"; stateButton: ScreenMsgProc.state_set2_S1_14 == "30m" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_powersaving_01h; x: 50; y: 242; filePath: "image://image_provider/:/Set3_S2_PowerSaving_3"; strCmd: "PowerSaving"; strParam: "01h"; stateButton: ScreenMsgProc.state_set2_S1_14 == "01h" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_powersaving_02h; x: 420; y: 242; filePath: "image://image_provider/:/Set3_S2_PowerSaving_4"; strCmd: "PowerSaving"; strParam: "02h"; stateButton: ScreenMsgProc.state_set2_S1_14 == "02h" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_powersaving_03h; x: 50; y: 316; filePath: "image://image_provider/:/Set3_S2_PowerSaving_5"; strCmd: "PowerSaving"; strParam: "03h"; stateButton: ScreenMsgProc.state_set2_S1_14 == "03h" ? "S" : "N" }
}
