import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_input_selectmedia_sdvideoplayer
    width: 800
    height: 480
    color: "#00000000" //透明

    property string playtime: "00:38:54"

    //背景
    Image {
        id: image_background
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_VideoPlay_bg.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentInputSelectMediaSDVideo }

    //バー
    Image {
        x: 70
        y: 355
        width: 660
        height: 12
        source: "images/PXOSD_R0_Memory_V_PlayBar.png"
    }
    Rectangle {
        id: ellipse_left
        width: 10
        height: width
        x: 70 + 1
        y: 355 + 1
        color: define.colorBar
        border.color:  define.colorBar
        border.width: 1
        radius: width*0.5
    }
    Rectangle {
        id: rect_bar
        width: image_current.x - 70
        height: 10
        x: ellipse_left.x + (ellipse_left.width / 2)
        y: ellipse_left.y
        color: define.colorBar
    }
    Image {
        id: image_current
        x: 55 + ScreenMsgProc.state_input_sdvideo * 42.9
        y: 341
        width: 42
        height: 42
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_PlayTag_OFF.png"

        MouseArea {
            anchors.fill: parent
            onPressed: {
                if (image_current.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_PlayTag_OFF.png") {
                    image_current.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_PlayTag_ON.png"
                }
            }
            onMouseXChanged: {
                image_current.x = image_current.x + mouseX - 21
                if (image_current.x < 55) {
                    image_current.x = 55
                } else if (703 < image_current.x) {
                    image_current.x = 703
                }
            }
            onReleased: {
                if (image_current.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_PlayTag_ON.png") {
                    image_current.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_PlayTag_OFF.png"
                }
            }
        }
    }
    //ボタン
    Image {
        id: image_r
        x: 288
        y: 400
        width: 60
        height: 60
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_R_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_r.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_R_P.png"
            }
            onContainsMouseChanged: {
                image_r.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_R_N.png"
            }
            onReleased: {
                if (image_r.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_R_P.png") {
                    image_r.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_R_N.png"
                    ScreenMsgProc.video_control = "SDvideoPlay:R"
                }
            }
        }
    }
    Image {
        id: image_play_pause
        x: 360
        y: 390
        width: 80
        height: 80
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Pause_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                if (image_play_pause.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Pause_N.png") {
                    image_play_pause.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Pause_P.png"
                }
                if (image_play_pause.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Play_N.png") {
                    image_play_pause.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Play_P.png"
                }
            }
            onContainsMouseChanged: {
                if (image_play_pause.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Pause_P.png") {
                    image_play_pause.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Pause_N.png"
                }
                if (image_play_pause.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Play_P.png") {
                    image_play_pause.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Play_N.png"
                }
            }
            onReleased: {
                if (image_play_pause.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Pause_P.png") {
                    image_play_pause.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Play_N.png"
                    ScreenMsgProc.video_control = "SDvideoPlay:Pause"
                }
                if (image_play_pause.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Play_P.png") {
                    image_play_pause.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Pause_N.png"
                    ScreenMsgProc.video_control = "SDvideoPlay:Play"
                }
            }
        }
    }
    Image {
        id: image_f
        x: 452
        y: 400
        width: 60
        height: 60
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_F_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_f.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_F_P.png"
            }
            onContainsMouseChanged: {
                image_f.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_F_N.png"
            }
            onReleased: {
                if (image_f.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_F_P.png") {
                    image_f.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_F_N.png"
                    ScreenMsgProc.video_control = "SDvideoPlay:F"
                }
            }
        }
    }
    Image {
        id: image_repeat
        x: 730
        y: 400
        width: 60
        height: 60
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Repeat_N.png"
        MouseArea {
            anchors.fill: parent

            onPressed: {
                if (image_repeat.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Repeat_N.png") {
                    image_repeat.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Repeat_P.png"
                }
                if (image_repeat.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_NonRepeat_N.png") {
                    image_repeat.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_NonRepeat_P.png"
                }
            }
            onContainsMouseChanged: {
                if (image_repeat.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Repeat_P.png") {
                    image_repeat.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Repeat_N.png"
                }
                if (image_repeat.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_NonRepeat_P.png") {
                    image_repeat.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_NonRepeat_N.png"
                }
            }
            onReleased: {
                if (image_repeat.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Repeat_P.png") {
                    image_repeat.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_NonRepeat_N.png"
                    ScreenMsgProc.video_control = "SDvideoPlay:NonRepeat"
                }
                if (image_repeat.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_NonRepeat_P.png") {
                    image_repeat.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Repeat_N.png"
                    ScreenMsgProc.video_control = "SDvideoPlay:Repeat"
                }
            }
        }
    }
    Image {
        id: image_vol_bg
        x: 744
        y: 153
        width: 46
        height: 136
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Vol_bg.png"
    }
    Image {
        id: image_volup
        x: 744
        y: 153
        width: 46
        height: 46
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_VolUP_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_volup.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_VolUP_P.png"
            }
            onContainsMouseChanged: {
                image_volup.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_VolUP_N.png"
            }
            onReleased: {
                if (image_volup.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_VolUP_P.png") {
                    image_volup.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_VolUP_N.png"
                    ScreenMsgProc.video_control = "SDvideoPlay:VolUP"
                }
            }
        }
    }
    Image {
        id: image_voldown
        x: 744
        y: 243
        width: 46
        height: 46
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_VolDOWN_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_voldown.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_VolDOWN_P.png"
            }
            onContainsMouseChanged: {
                image_voldown.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_VolDOWN_N.png"
            }
            onReleased: {
                if (image_voldown.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_VolDOWN_P.png") {
                    image_voldown.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_VolDOWN_N.png"
                    ScreenMsgProc.video_control = "SDvideoPlay:VolDOWN"
                }
            }
        }
    }

    //再生時間
    //Text { id: text_playtime; x: 40; y: 400; color: "white" ; font.pixelSize: 30; text: playtime }
    Text { id: text_playtime; x: 40; y: 415; color: "white" ; font.pixelSize: 30; text: playtime }
}
