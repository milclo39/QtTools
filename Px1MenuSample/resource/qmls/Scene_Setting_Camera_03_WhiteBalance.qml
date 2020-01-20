import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_camera_whitebalance
    width: 800
    height: 480

    function setNormal()
    {
        button_setting3_whitebalance_auto.setNormal()
        button_setting3_whitebalance_onepush.setNormal()
        button_setting3_whitebalance_flourescent.setNormal()
        button_setting3_whitebalance_indoors.setNormal()
        button_setting3_whitebalance_outdoors.setNormal()
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
        source: "qrc:///Set3_C2_WhiteBalance_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingCamera }

    //設定ボタン
    Button_Setting3 { id: button_setting3_whitebalance_auto; x: 0; y: 94;  filePath: "image://image_provider/:/Set3_C2_WhiteBalance_1"; strCmd: "WhiteBalance"; strParam: "Auto"; stateButton: ScreenMsgProc.state_set2_C1_3 == "Auto" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_whitebalance_onepush; x: 0; y: 168; filePath: "image://image_provider/:/Set3_C2_WhiteBalance_2"; strCmd: "WhiteBalance"; strParam: "One-push"; stateButton: ScreenMsgProc.state_set2_C1_3 == "Onepush" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_whitebalance_flourescent; x: 0; y: 242; filePath: "image://image_provider/:/Set3_C2_WhiteBalance_3"; strCmd: "WhiteBalance"; strParam: "Flourescent"; stateButton: ScreenMsgProc.state_set2_C1_3 == "FluorescentLamp" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_whitebalance_indoors; x: 0; y: 316; filePath: "image://image_provider/:/Set3_C2_WhiteBalance_4"; strCmd: "WhiteBalance"; strParam: "Indoors"; stateButton: ScreenMsgProc.state_set2_C1_3 == "Indoor" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_whitebalance_outdoors; x: 0; y: 390; filePath: "image://image_provider/:/Set3_C2_WhiteBalance_5"; strCmd: "WhiteBalance"; strParam: "Outdoors"; stateButton: ScreenMsgProc.state_set2_C1_3 == "Outdoor" ? "S" : "N" }
}
