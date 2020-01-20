import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_system_resolution
    width: 800
    height: 480

    function setNormal()
    {
        button_setting3_resolution_auto.setNormal()
        button_setting3_resolution_hd.setNormal()
        button_setting3_resolution_uhd.setNormal()
        button_setting3_resolution_uxga.setNormal()
        button_setting3_resolution_fhd.setNormal()
        button_setting3_resolution_xga.setNormal()
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
        source: "qrc:///Set3_S2_Resolution_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    //設定ボタン
    Button_Setting3 { id: button_setting3_resolution_auto; x: 50; y: 168; filePath: "image://image_provider/:/Set3_S2_Resolution_1"; strCmd: "Resolution"; strParam: "Auto"; stateButton: ScreenMsgProc.state_set2_S1_01 == "Auto" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_resolution_hd; x: 420; y: 168; filePath: "image://image_provider/:/Set3_S2_Resolution_2"; strCmd: "Resolution"; strParam: "HD"; stateButton: ScreenMsgProc.state_set2_S1_01 == "HD" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_resolution_uhd; x: 50; y: 242; filePath: "image://image_provider/:/Set3_S2_Resolution_3"; strCmd: "Resolution"; strParam: "UHD"; stateButton: ScreenMsgProc.state_set2_S1_01 == "UHD" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_resolution_uxga; x: 420; y: 242; filePath: "image://image_provider/:/Set3_S2_Resolution_4"; strCmd: "Resolution"; strParam: "UXGA"; stateButton: ScreenMsgProc.state_set2_S1_01 == "UXGA" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_resolution_fhd; x: 50; y: 316; filePath: "image://image_provider/:/Set3_S2_Resolution_5"; strCmd: "Resolution"; strParam: "FHD"; stateButton: ScreenMsgProc.state_set2_S1_01 == "FHD" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_resolution_xga; x: 420; y: 316; filePath: "image://image_provider/:/Set3_S2_Resolution_6"; strCmd: "Resolution"; strParam: "XGA"; stateButton: ScreenMsgProc.state_set2_S1_01 == "XGA" ? "S" : "N" }
}
