import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_system_language
    width: 800
    height: 480

    function setNormal()
    {
        button_setting3_language_j.setNormal()
        button_setting3_language_e.setNormal()
        button_setting3_language_d.setNormal()
        button_setting3_language_f.setNormal()
        button_setting3_language_a.setNormal()
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
        source: "qrc:///Set3_S2_language_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    //設定ボタン
    Button_Setting3 { id: button_setting3_language_j; x: 50; y: 168; filePath: "image://image_provider/:/Set3_S2_language_1"; strCmd: "Language"; strParam: "J"; stateButton: ScreenMsgProc.state_set2_S1_15 == "J" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_language_e; x: 420; y :168; filePath: "image://image_provider/:/Set3_S2_language_2"; strCmd: "Language"; strParam: "E"; stateButton: ScreenMsgProc.state_set2_S1_15 == "E" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_language_d; x: 50; y: 242; filePath: "image://image_provider/:/Set3_S2_language_3"; strCmd: "Language"; strParam: "D"; stateButton: ScreenMsgProc.state_set2_S1_15 == "D" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_language_f; x: 420; y: 242; filePath: "image://image_provider/:/Set3_S2_language_4"; strCmd: "Language"; strParam: "F"; stateButton: ScreenMsgProc.state_set2_S1_15 == "F" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_language_a; x: 50; y: 316; filePath: "image://image_provider/:/Set3_S2_language_5"; strCmd: "Language"; strParam: "A"; stateButton: ScreenMsgProc.state_set2_S1_15 == "A" ? "S" : "N" }
}
