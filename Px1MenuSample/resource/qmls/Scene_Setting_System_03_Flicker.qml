﻿import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_system_flicker
    width: 800
    height: 480

    function setNormal()
    {
        button_setting3_flicker_50hz.setNormal()
        button_setting3_flicker_60hz.setNormal()
        button_setting3_flicker_auto.setNormal()
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
        source: "qrc:///Set3_S2_Flicker_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    //設定ボタン
    Button_Setting3 { id: button_setting3_flicker_50hz; x: 50; y: 168; filePath: "image://image_provider/:/Set3_S2_Flicker_1"; strCmd: "Flicker"; strParam: "50Hz"; stateButton: ScreenMsgProc.state_set2_S1_03 == "50Hz" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_flicker_60hz; x: 420; y: 168; filePath: "image://image_provider/:/Set3_S2_Flicker_2"; strCmd: "Flicker"; strParam: "60Hz"; stateButton: ScreenMsgProc.state_set2_S1_03 == "60Hz" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_flicker_auto; x: 50; y: 242; filePath: "image://image_provider/:/Set3_S2_Flicker_3"; strCmd: "Flicker"; strParam: "Auto"; stateButton: ScreenMsgProc.state_set2_S1_03 == "Auto" ? "S" : "N" }
}
