import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_brightness
    width: 800
    height: 480
    color: "#00000000" //透明

    //背景
    Image {
        id: image_background
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Background1.png"
    }

    //タイトル
    Image {
        id: image_title
        x: 0
        y: 0
        width: 800
        height: 60
        source: "qrc:///Brightness_title.png"
    }

    //閉じるボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back1"; component: componentBrightness }

    Image {
        x: 0
        y: 370
        source: "qrc:///Brightness_underbar.png"
    }
    Image {
        x: 50
        y: 429
        source: ScreenMsgProc.state_brightness_lamp == true ? "image://image_provider/:/qmls/images/PXOSD_R0_Lamp_ON.png" : "image://image_provider/:/qmls/images/PXOSD_R0_Lamp_OFF.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (ScreenMsgProc.state_brightness_lamp == true) {
                    ScreenMsgProc.state_brightness_lamp = false
                } else {
                    ScreenMsgProc.state_brightness_lamp = true
                }
            }
        }
    }
    Image {
        id: image_default
        x: 199
        y: 420
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
                    ScreenMsgProc.state_brightness_aperture = 9
                    slider.value = 9
                }
            }
        }
    }
    Image {
        x: 675
        y: 420
        source: ScreenMsgProc.state_brightness_antireflection == true ? "image://image_provider/:/qmls/images/PXOSD_R0_Antireflection_ON.png" : "image://image_provider/:/qmls/images/PXOSD_R0_Antireflection_OFF.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (ScreenMsgProc.state_brightness_antireflection == true) {
                    ScreenMsgProc.state_brightness_antireflection = false
                } else {
                    ScreenMsgProc.state_brightness_antireflection = true
                }
            }
        }
    }

    //スライダー
    Slider {
        id: slider
        maximumValue: 18.0
        minimumValue: 0.0
        stepSize: 1.0
        value: ScreenMsgProc.state_brightness_aperture
        x: 268
        y: 419
        style: SliderStyle {
            groove: Image {
                width: 312
                height: 12
                source: "image://image_provider/:/qmls/images/PXOSD_R0_Aperture_bar.png"
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
                    source: control.pressed ? "image://image_provider/:/qmls/images/PXOSD_R0_Aperture_ON.png" : "image://image_provider/:/qmls/images/PXOSD_R0_Aperture_OFF.png"
                }
            }
        }
        onValueChanged: {
            ScreenMsgProc.state_brightness_aperture = slider.value
        }
    }
}
