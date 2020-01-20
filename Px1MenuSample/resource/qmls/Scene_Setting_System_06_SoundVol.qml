import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_system_soundvol
    width: 800
    height: 480

    property string paramCurrent: ""

    function changeCurrentText()
    {
        if (slider.value > 9) {
            ScreenMsgProc.state_set2_S1_06 = slider.value.toString()
            image_current_text1.source = "images/PXOSD_R0_TEXT2_0.png"
        } else {
            ScreenMsgProc.state_set2_S1_06 = "0" + slider.value.toString()
            image_current_text1.source = "images/PXOSD_R0_TEXT2_" + slider.value + ".png"
        }
        visibleCurentText()
        if (paramCurrent != ScreenMsgProc.state_set2_S1_06) {
            paramCurrent = ScreenMsgProc.state_set2_S1_06
        }
        return image_current_text1.source
    }
    function visibleCurentText()
    {
        image_current_text2.visible = slider.value > 9 ? true : false
        return image_current_text2.visible
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
        source: "qrc:///Set3_S2_SoundVol_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystem }

    Image {
        x: 300
        y: 176
        source: "qrc:///Set3_S2_SoundVol_bg1.png"
    }
    Image {
        x: 0
        y: 328
        source: "qrc:///Set3_S2_SoundVol_bg2.png"
    }
    Image {
        x: 730
        y: 328
        source: "qrc:///Set3_S2_SoundVol_bg3.png"
    }
    Image {
        id: image_current_text1
        x: 416
        y: 188
        visible: true
        source: changeCurrentText()
    }
    Image {
        id: image_current_text2
        x: 387
        y: 188
        visible: visibleCurentText()
        source: "images/PXOSD_R0_TEXT2_1.png"
    }

    //スライダー
    Slider {
        id: slider
        maximumValue: 10.0
        minimumValue: 0.0
        stepSize: 1.0
        value: ScreenMsgProc.state_set2_S1_06
        x: 70
        y: 327
        style: SliderStyle {
            groove: Image {
                width: 660
                height: 12
                source: "qrc:///Set3_S2_SoundVol_Levelbar.png"
                Rectangle {
                    height: parent.height
                    width: styleData.handlePosition
                    radius: height/2
                    color: define.colorBar
                }
            }
            handle: Item {
                implicitWidth: 36
                implicitHeight: 42
                Image {
                    anchors.centerIn: parent
                    source: control.pressed ? "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_PlayTag_ON.png" : "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_PlayTag_OFF.png"
                }
            }
        }
        onValueChanged: {
            changeCurrentText()
        }
    }
}
