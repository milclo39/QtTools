import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_camera
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
        source: "qrc:///Set2_S1_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    Rectangle {
        x: 30
        y: 94
        width: 750
        height: (50 + 24) * 5

        Flickable {
            id: flick
            anchors.fill: parent
            contentWidth: 750
            contentHeight: (50 + 24) * 5 //ボタン数=24
            clip: true
            state: 'menu1'

            Grid {
                id: grid_button_1
                columns: 1
                spacing: 24
                visible: true
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_01"; state: ScreenMsgProc.state_set2_S1_01; component: componentSettingSystemResolution }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_02"; state: ScreenMsgProc.state_set2_S1_02; component: componentSettingSystemAspectRatio }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_03"; state: ScreenMsgProc.state_set2_S1_03; component: componentSettingSystemFlicker }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_04"; state: ScreenMsgProc.state_set2_S1_04; component: componentSettingSystemUSBMode }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_05"; state: ScreenMsgProc.state_set2_S1_05; component: componentSettingSystemAudioSwitch }
            }
            Grid {
                id: grid_button_2
                columns: 1
                spacing: 24
                visible: false
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_06"; state: ScreenMsgProc.state_set2_S1_06; component: componentSettingSystemSoundVol }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_07"; state: ScreenMsgProc.state_set2_S1_07; component: componentSettingSystemOpSoundVol }
                //Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_08"; state: ScreenMsgProc.state_set2_S1_08; component: componentSettingSystemRecSize } //del
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_09"; state: ScreenMsgProc.state_set2_S1_09; component: componentSettingSystemRecQuality }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_10"; state: ScreenMsgProc.state_set2_S1_10; component: componentSettingSystemRecCompression }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_11"; state: ScreenMsgProc.state_set2_S1_11; component: componentSettingSystemRecMode } //add
            }
            Grid {
                id: grid_button_3
                columns: 1
                spacing: 24
                visible: false
                //Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_11"; state: ScreenMsgProc.state_set2_S1_11; component: componentSettingSystemRecMode } //del
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_12"; state: ScreenMsgProc.state_set2_S1_12; component: componentSettingSystemTimelapseInterval }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_13"; state: ScreenMsgProc.state_set2_S1_13; component: componentSettingSystemBacklight }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_14"; state: ScreenMsgProc.state_set2_S1_14; component: componentSettingSystemPowerSaving }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_15"; state: ScreenMsgProc.state_set2_S1_15; component: componentSettingSystemLanguage }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_16"; state: ""; component: componentSettingSystemInitialize } //add
            }
            Grid {
                id: grid_button_4
                columns: 1
                spacing: 24
                visible: false
                //Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_16"; state: ""; component: componentSettingSystemInitialize } //del
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_17"; state: ""; component: componentSettingSystemSDFormat }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_18"; state: ""; component: componentSettingSystemUSBFormat }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_19"; state: ScreenMsgProc.state_set2_S1_19; component: componentSettingSystemTimeset }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_20"; state: ScreenMsgProc.state_set2_S1_20; component: componentSettingSystemRotation } //add
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_21"; state: ScreenMsgProc.state_set2_S1_21; component: componentSettingSystemFreeze } //add
            }
            Grid {
                id: grid_button_5
                columns: 1
                spacing: 24
                visible: false
                //Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_20"; state: ScreenMsgProc.state_set2_S1_20; component: componentSettingSystemRotation } //del
                //Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_21"; state: ScreenMsgProc.state_set2_S1_21; component: componentSettingSystemFreeze }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_22"; state: ScreenMsgProc.state_set2_S1_22; component: componentSettingSystemMemory }
                Button_Setting2 { filePath: "image://image_provider/:/Set2_S1_23"; state: ScreenMsgProc.state_set2_S1_23; component: componentSettingSystemHDCP }
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
                            } else if (flick.state == 'menu3') {
                                flick.state = 'menu2'
                            } else if (flick.state == 'menu4') {
                                flick.state = 'menu3'
                            } else if (flick.state == 'menu5') {
                                flick.state = 'menu4'
                            }
                        }
                    }
                }
            }
            Image {
                id: down
                x: 690; y: 310
                source: "image://image_provider/:/Set3_S2_Timeset_DownN.png"
                visible: flick.state != 'menu5' ? true : false
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
                            } else if (flick.state == 'menu2') {
                                flick.state = 'menu3'
                            } else if (flick.state == 'menu3') {
                                flick.state = 'menu4'
                            } else if (flick.state == 'menu4') {
                                flick.state = 'menu5'
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
                    PropertyChanges { target: grid_button_3; visible: false  }
                    PropertyChanges { target: grid_button_4; visible: false  }
                    PropertyChanges { target: grid_button_5; visible: false  }
                },
                State {
                    name: "menu2"
                    PropertyChanges { target: grid_button_1; visible: false  }
                    PropertyChanges { target: grid_button_2; visible: true  }
                    PropertyChanges { target: grid_button_3; visible: false  }
                    PropertyChanges { target: grid_button_4; visible: false  }
                    PropertyChanges { target: grid_button_5; visible: false  }
                },
                State {
                    name: "menu3"
                    PropertyChanges { target: grid_button_1; visible: false  }
                    PropertyChanges { target: grid_button_2; visible: false  }
                    PropertyChanges { target: grid_button_3; visible: true  }
                    PropertyChanges { target: grid_button_4; visible: false  }
                    PropertyChanges { target: grid_button_5; visible: false  }
                },
                State {
                    name: "menu4"
                    PropertyChanges { target: grid_button_1; visible: false  }
                    PropertyChanges { target: grid_button_2; visible: false  }
                    PropertyChanges { target: grid_button_3; visible: false  }
                    PropertyChanges { target: grid_button_4; visible: true  }
                    PropertyChanges { target: grid_button_5; visible: false  }
                },
                State {
                    name: "menu5"
                    PropertyChanges { target: grid_button_1; visible: false  }
                    PropertyChanges { target: grid_button_2; visible: false  }
                    PropertyChanges { target: grid_button_3; visible: false  }
                    PropertyChanges { target: grid_button_4; visible: false  }
                    PropertyChanges { target: grid_button_5; visible: true  }
                }
            ]
        }
    }
}
