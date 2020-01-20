import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_camera_imagemode
    width: 800
    height: 480

    function setNormal()
    {
        button_setting3_imagemode_standard.setNormal()
        button_setting3_imagemode_landscape.setNormal()
        button_setting3_imagemode_wb.setNormal()
        button_setting3_imagemode_color.setNormal()
        button_setting3_imagemode_dlp.setNormal()
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
        source: "qrc:///Set3_C2_ImageMode_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingCamera }

    //設定ボタン
    Button_Setting3 { id: button_setting3_imagemode_standard; x: 0; y: 94;  filePath: "image://image_provider/:/Set3_C2_ImageMode_1"; strCmd: "ImageMode"; strParam: "Standard"; stateButton: ScreenMsgProc.state_set2_C1_4 == "Standard" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_imagemode_landscape; x: 0; y: 168; filePath: "image://image_provider/:/Set3_C2_ImageMode_2"; strCmd: "ImageMode"; strParam: "Landscape"; stateButton: ScreenMsgProc.state_set2_C1_4 == "Landscape" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_imagemode_wb; x: 0; y: 242; filePath: "image://image_provider/:/Set3_C2_ImageMode_3"; strCmd: "ImageMode"; strParam: "Black/white"; stateButton: ScreenMsgProc.state_set2_C1_4 == "WB" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_imagemode_color; x: 0; y: 316; filePath: "image://image_provider/:/Set3_C2_ImageMode_4"; strCmd: "ImageMode"; strParam: "Color emphasis"; stateButton: ScreenMsgProc.state_set2_C1_4 == "ColorEmphasis" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_imagemode_dlp; x: 0; y: 390; filePath: "image://image_provider/:/Set3_C2_ImageMode_5"; strCmd: "ImageMode"; strParam: "DLP"; stateButton: ScreenMsgProc.state_set2_C1_4 == "DLP" ? "S" : "N" }
}
