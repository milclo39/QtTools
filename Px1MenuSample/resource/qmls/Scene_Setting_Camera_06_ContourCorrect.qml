import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_camera_contourcorrect
    width: 800
    height: 480
    color: "#00000000" //透明

    property string paramCurrent: ""

    function changeCurrentText()
    {
        if (slider.value > 9) {
            ScreenMsgProc.state_set2_C1_6 = slider.value.toString()
            image_current_text1.source = "images/PXOSD_R0_TEXT2_0.png"
        } else {
            ScreenMsgProc.state_set2_C1_6 = "0" + slider.value.toString()
            image_current_text1.source = "images/PXOSD_R0_TEXT2_" + slider.value + ".png"
        }
        visibleCurentText()
        if (paramCurrent != ScreenMsgProc.state_set2_C1_6) {
            paramCurrent = ScreenMsgProc.state_set2_C1_6
        }
        return image_current_text1.source
    }
    function visibleCurentText()
    {
        image_current_text2.visible = slider.value > 9 ? true : false
        return image_current_text2.visible
    }

    //タイトル
    Image {
        id: image_title
        x: 0
        y: 0
        width: 800
        height: 60
        source: "qrc:///Set3_C2_ContourCorrect_title.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingCamera }

    Image {
        x: 0
        y: 400
        width: 800
        height: 80
        source: "qrc:///Set3_C2_ContourCorrect_btbar.png"
    }
    Image {
        id: image_default
        x: 85
        y: 420
        width: 42
        height: 42
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Default_OFF.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_default.source = "image://image_provider/:/qmls/images/PXOSD_R0_Default_ON.png"
            }
            onContainsMouseChanged: {
                image_default.source = "image://image_provider/:/qmls/images/PXOSD_R0_Default_OFF.png"
            }
            onReleased: {
                if (image_default.source == "image://image_provider/:/qmls/images/PXOSD_R0_Default_ON.png") {
                    image_default.source = "image://image_provider/:/qmls/images/PXOSD_R0_Default_OFF.png"
                    ScreenMsgProc.state_set2_C1_6 = "05"
                    image_current_text1.source = "images/PXOSD_R0_TEXT2_5.png"
                    image_current_text2.visible = false
                    paramCurrent = ScreenMsgProc.state_set2_C1_6
                    slider.value = 5
                }
            }
        }
    }
    Image {
        id: image_current_text1
        x: 759
        y: 414
        visible: true
        source: changeCurrentText()
    }
    Image {
        id: image_current_text2
        x: 730
        y: 414
        visible: visibleCurentText()
        source: "images/PXOSD_R0_TEXT2_1.png"
    }

    //スライダー
    Slider {
        id: slider
        maximumValue: 10.0
        minimumValue: 0.0
        stepSize: 1.0
        value: ScreenMsgProc.state_set2_C1_6
        x: 165
        y: 420
        style: SliderStyle {
            groove: Image {
                width: 472
                height: 12
                source: "qrc:///Set3_C2_ContourCorrect_LevelBar.png"
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
