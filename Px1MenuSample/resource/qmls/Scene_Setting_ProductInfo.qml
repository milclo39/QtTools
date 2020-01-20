import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_productinfo
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
        source: "qrc:///Set2_P1_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingProductInfo }

    //ボタンリスト
    Rectangle {
        x: 30
        y: 94
        width: 750
        height: (50 + 24) * 5

        Grid {
            id: grid_button
            columns: 1
            spacing: 24

            Button_Setting2 { filePath: "image://image_provider/:/Set2_P1_1"; state: ScreenMsgProc.state_set2_P1_1; component: componentSettingProductInfoCamera }
            Button_Setting2 { filePath: "image://image_provider/:/Set2_P1_2"; state: ScreenMsgProc.state_set2_P1_2; component: componentSettingProductInfoNetwork }
            Button_Setting2 { filePath: "image://image_provider/:/Set2_P1_3"; state: ScreenMsgProc.state_set2_P1_3; component: componentSettingProductInfoLicense }
        }
    }
}
