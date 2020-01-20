import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_productinfo_camera
    width: 800
    height: 480

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
        source: "qrc:///Set3_P2_CameraInfo_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    Image {
        id: image_back
        x: 147
        y: 137
        source: "qrc:///Set3_S2_Version_bg.png"
    }

    //バージョン情報
    Grid {
        id: grid_version
        columns: 1
        rows: 3
        anchors.centerIn: image_back
        spacing: 20
        Text { color: "white" ; font.pixelSize: 31; text: ScreenMsgProc.state_set2_P1_3_main }
        Text { color: "white" ; font.pixelSize: 31; text: ScreenMsgProc.state_set2_P1_3_m0 }
        Text { color: "white" ; font.pixelSize: 31; text: ScreenMsgProc.state_set2_P1_3_fpga }
    }
}
