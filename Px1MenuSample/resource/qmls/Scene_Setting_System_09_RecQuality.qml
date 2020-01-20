import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_system_recquality
    width: 800
    height: 480

    function setNormal()
    {
/*
        button_setting3_recquality_high.setNormal()
        button_setting3_recquality_medium.setNormal()
        button_setting3_recquality_low.setNormal()
*/
        button_setting3_recquality_superfine.setNormal()
        button_setting3_recquality_fine.setNormal()
        button_setting3_recquality_standard.setNormal()
        button_setting3_recquality_basic.setNormal()
        button_setting3_recquality_economy.setNormal()
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
        source: "qrc:///Set3_S2_RecQuality_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    //設定ボタン
/*
    Button_Setting3 { id: button_setting3_recquality_high; x: 50; y: 168; filePath: "image://image_provider/:/Set3_S2_RecSize_Quality_1"; strCmd: "RecQuality"; strParam: "High"; stateButton: ScreenMsgProc.state_set2_S1_09 == "High" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_recquality_medium; x: 420; y: 168; filePath: "image://image_provider/:/Set3_S2_RecSize_Quality_2"; strCmd: "RecQuality"; strParam: "Medium"; stateButton: ScreenMsgProc.state_set2_S1_09 == "Medium" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_recquality_low; x: 50; y: 242; filePath: "image://image_provider/:/Set3_S2_RecSize_Quality_3"; strCmd: "RecQuality"; strParam: "Low"; stateButton: ScreenMsgProc.state_set2_S1_09 == "Low" ? "S" : "N" }
*/
    Button_Setting3 { id: button_setting3_recquality_superfine; x: 50; y: 168; filePath: "image://image_provider/:/Set3_S2_RecQuality_1"; strCmd: "RecQuality"; strParam: "SuperFine"; stateButton: ScreenMsgProc.state_set2_S1_09 == "SuperFine" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_recquality_fine; x: 420; y: 168; filePath: "image://image_provider/:/Set3_S2_RecQuality_2"; strCmd: "RecQuality"; strParam: "Fine"; stateButton: ScreenMsgProc.state_set2_S1_09 == "Fine" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_recquality_standard; x: 50; y: 242; filePath: "image://image_provider/:/Set3_S2_RecQuality_3"; strCmd: "RecQuality"; strParam: "Standard"; stateButton: ScreenMsgProc.state_set2_S1_09 == "Standard" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_recquality_basic; x: 420; y: 242; filePath: "image://image_provider/:/Set3_S2_RecQuality_4"; strCmd: "RecQuality"; strParam: "Basic"; stateButton: ScreenMsgProc.state_set2_S1_09 == "Basic" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_recquality_economy; x: 50; y: 316; filePath: "image://image_provider/:/Set3_S2_RecQuality_5"; strCmd: "RecQuality"; strParam: "Economy"; stateButton: ScreenMsgProc.state_set2_S1_09 == "Economy" ? "S" : "N" }
}
