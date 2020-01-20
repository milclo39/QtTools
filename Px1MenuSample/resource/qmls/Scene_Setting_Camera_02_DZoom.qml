import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_camera_dzoom
    width: 800
    height: 480

    function setNormal()
    {
        button_setting3_dzoom_on.setNormal()
        button_setting3_dzoom_off.setNormal()
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
        source: "qrc:///Set3_C2_Dzoom_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingCamera }

    //設定ボタン
    Button_Setting3 { id: button_setting3_dzoom_on; x: 50; y: 242;  filePath: "image://image_provider/:/Set3_C2_Dzoom_1"; strCmd: "DZoom"; strParam: "ON"; stateButton: ScreenMsgProc.state_set2_C1_2 == "ON" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_dzoom_off; x: 420; y: 242; filePath: "image://image_provider/:/Set3_C2_Dzoom_2"; strCmd: "DZoom"; strParam: "OFF"; stateButton: ScreenMsgProc.state_set2_C1_2 == "OFF" ? "S" : "N" }
}
