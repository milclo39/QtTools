import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_camera_af
    width: 800
    height: 480

    function setNormal()
    {
        button_setting3_af_zoomsync.setNormal()
        button_setting3_af_onepush.setNormal()
        button_setting3_af_continuous.setNormal()
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
        source: "qrc:///Set3_C2_AF_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingCamera }

    //設定ボタン
    Button_Setting3 { id: button_setting3_af_zoomsync; x: 50; y: 168; filePath: "image://image_provider/:/Set3_C2_AF_1"; strCmd: "AF"; strParam: "Zoom sync"; stateButton: ScreenMsgProc.state_set2_C1_1 == "Zoomsync" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_af_onepush; x: 420; y :168; filePath: "image://image_provider/:/Set3_C2_AF_2"; strCmd: "AF"; strParam: "One-push"; stateButton: ScreenMsgProc.state_set2_C1_1 == "Onepush" ? "S" : "N" }
    Button_Setting3 { id: button_setting3_af_continuous; x: 50; y: 242; filePath: "image://image_provider/:/Set3_C2_AF_3"; strCmd: "AF"; strParam: "Continuous"; stateButton: ScreenMsgProc.state_set2_C1_1 == "Continuous" ? "S" : "N" }
}
