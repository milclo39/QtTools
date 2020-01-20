import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_input
    width: 800
    height: 480
    color: "#00000000" //透明

    function setNormal()
    {
        button_input_camera.setNormal()
        button_input_selectmedia.setNormal()
        button_input_hdmi1.setNormal()
        button_input_hdmi2.setNormal()
        button_input_rgb.setNormal()
    }

    //背景
    Image {
        id: image_background
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Background1.png"
    }

    //タイトル
    Image {
        id: image_title
        x: 0
        y: 0
        width: 800
        height: 60
        source: "qrc:///INPUT1_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back1"; component: componentInput }

    //設定ボタン
    Button_Setting3 { id: button_input_camera; x: 50; y:168; filePath: "image://image_provider/:/INPUT1_1"; strCmd: "Input"; strParam: "Camera"; stateButton: ScreenMsgProc.state_input == "Camera" ? "S" : "N" }
    Button_Setting3 { id: button_input_selectmedia; x: 420; y:168; filePath: "image://image_provider/:/INPUT1_2"; strCmd: "Input"; strParam: "SelectMedia"; stateButton: ScreenMsgProc.state_input == "SelectMedia" ? "S" : "N" }
    Button_Setting3 { id: button_input_hdmi1; x: 50; y:242; filePath: "image://image_provider/:/INPUT1_3"; strCmd: "Input"; strParam: "HDMI1"; stateButton: ScreenMsgProc.state_input == "HDMI1" ? "S" : "N" }
    Button_Setting3 { id: button_input_hdmi2; x: 420; y:242; filePath: "image://image_provider/:/INPUT1_4"; strCmd: "Input"; strParam: "HDMI2"; stateButton: ScreenMsgProc.state_input == "HDMI2" ? "S" : "N" }
    Button_Setting3 { id: button_input_rgb; x: 50; y:316; filePath: "image://image_provider/:/INPUT1_5"; strCmd: "Input"; strParam: "RGB"; stateButton: ScreenMsgProc.state_input == "RGB" ? "S" : "N" }
}
