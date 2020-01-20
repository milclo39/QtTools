import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_system
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
        source: "qrc:///Set2_C1_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingCamera }

    Rectangle {
        x: 30
        y: 94
        width: 750
        height: (50 + 24) * 5

        Flickable {
            id: flick
            anchors.fill: parent
            contentWidth: 750
            contentHeight: (50 + 24) * 5
            clip: true
            state: 'menu1'

            Grid {
                id: grid_button_1
                columns: 1
                spacing: 24
                visible: true
                Button_Setting2 { filePath: "image://image_provider/:/Set2_C1_1"; state: ScreenMsgProc.state_set2_C1_1; component: componentSettingCameraAF }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_C1_2"; state: ScreenMsgProc.state_set2_C1_2; component: componentSettingCameraDZoom }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_C1_3"; state: ScreenMsgProc.state_set2_C1_3; component: componentSettingCameraWhiteBalance }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_C1_4"; state: ScreenMsgProc.state_set2_C1_4; component: componentSettingCameraImageMode }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_C1_5"; state: ""; component: componentSettingCameraMask }
            }
            Grid {
                id: grid_button_2
                columns: 1
                spacing: 24
                visible: false
                Button_Setting2 { filePath: "image://image_provider/:/Set2_C1_6"; state: ScreenMsgProc.state_set2_C1_6; component: componentSettingCameraContourCorrect }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_C1_7"; state: ScreenMsgProc.state_set2_C1_7; component: componentSettingCameraContrast }
            }
            Image {
                id: up
                x: 690; y: 0
                source: "image://image_provider/:/Set3_S2_Timeset_UpN.png"
                visible: flick.state != 'menu1' ? true : false
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        up.source = "image://image_provider/:/Set3_S2_Timeset_UpP.png"
                    }
                    onContainsMouseChanged: {
                        up.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
                    }
                    onClicked: {
                        if (up.source == "image://image_provider/:/Set3_S2_Timeset_UpP.png") {
                            up.source = "image://image_provider/:/Set3_S2_Timeset_UpN.png"
                            if (flick.state == 'menu2') {
                                flick.state = 'menu1'
                            }
                        }
                    }
                }
            }
            Image {
                id: down
                x: 690; y: 310
                source: "image://image_provider/:/Set3_S2_Timeset_DownN.png"
                visible: flick.state != 'menu2' ? true : false
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        down.source = "image://image_provider/:/Set3_S2_Timeset_DownP.png"
                    }
                    onContainsMouseChanged: {
                        down.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
                    }
                    onClicked: {
                        if (down.source == "image://image_provider/:/Set3_S2_Timeset_DownP.png") {
                            down.source = "image://image_provider/:/Set3_S2_Timeset_DownN.png"
                            if (flick.state == 'menu1') {
                                flick.state = 'menu2'
                            }
                        }
                    }
                }
            }
            states: [
                State {
                    name: "menu1"
                    PropertyChanges { target: grid_button_1; visible: true  }
                    PropertyChanges { target: grid_button_2; visible: false  }
                },
                State {
                    name: "menu2"
                    PropertyChanges { target: grid_button_1; visible: false  }
                    PropertyChanges { target: grid_button_2; visible: true  }
                }
            ]
        }
    }
}
