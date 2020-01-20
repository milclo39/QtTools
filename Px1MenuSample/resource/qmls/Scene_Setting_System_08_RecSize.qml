import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_system_recsize
    width: 800
    height: 480

    function setNormal()
    {
        button_setting3_recsize_high.setNormal()
        button_setting3_recsize_medium.setNormal()
        button_setting3_recsize_low.setNormal()
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
        source: "qrc:///Set3_S2_RecSize_Quality_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    //設定ボタン
    Button_Setting3 { id: button_setting3_recsize_high; x: 50; y: 168; filePath: "image://image_provider/:/Set3_S2_RecSize_Quality_1"; strCmd: "RecSize"; strParam: "High"; stateButton: ScreenMsgProc.state_set2_S1_08 == "High" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_recsize_medium; x: 420; y: 168; filePath: "image://image_provider/:/Set3_S2_RecSize_Quality_2"; strCmd: "RecSize"; strParam: "Medium"; stateButton: ScreenMsgProc.state_set2_S1_08 == "Medium" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_recsize_low; x: 50; y: 242; filePath: "image://image_provider/:/Set3_S2_RecSize_Quality_3"; strCmd: "RecSize"; strParam: "Low"; stateButton: ScreenMsgProc.state_set2_S1_08 == "Low" ? "S" : "N" }
}
