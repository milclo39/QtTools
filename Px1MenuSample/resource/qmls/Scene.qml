import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2


Rectangle {
    id: scene
    width: 800
    height: 480
    color: "#00000000" //透明

    //テスト用
    MouseArea {
        anchors.fill: parent
        onClicked: {
            grid_button.visible = true
            button_back.visible = true
        }
    }
    //閉じるボタン
    Image {
        id: button_back
        x: 740
        y: 0
        width: 60
        height: 60
        visible: false
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Back1N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                button_back.source = "image://image_provider/:/qmls/images/PXOSD_R0_Back1P.png"
            }
            onContainsMouseChanged: {
                button_back.source = "image://image_provider/:/qmls/images/PXOSD_R0_Back1N.png"
            }
            onReleased: {
                if (button_back.source == "image://image_provider/:/qmls/images/PXOSD_R0_Back1P.png") {
                    grid_button.visible = false
                    button_back.visible = false
                }
            }
        }
    }
    //ボタンリスト
    Rectangle {
        x:123; y: 0; width: 740 - 123 * 2; height: 90
        anchors.centerIn: parent
        color: "#00000000"
        Grid {
            id: grid_button
            columns: 3
            anchors.centerIn: parent
            spacing: 123
            visible: false
            Button_Show{ filePath: "image://image_provider/:/qmls/images/PXOSD_Show_1Brightness_"; component: componentBrightness }
            Button_Show{ filePath: "image://image_provider/:/qmls/images/PXOSD_Show_2Input_"; component: componentInput }
            Button_Show{ filePath: "image://image_provider/:/qmls/images/PXOSD_Show_3Setting_"; component: componentSetting }
        }
    }
}

//テスト用(1)
/*
Flickable {
    id: scene
    width: 800
    height: 480
    property int currentDuration: 2000
    Image {
        id: image_background
        x: 0; y: 0
        width: 800; height: 480
        //source: "images/background.jpg"
        source: "image://image_provider/:/qmls/images/background.jpg"
    }
    transform: Rotation {
        id: rotation
        origin.x: scene.width/2
        origin.y: scene.height/2
        axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
        angle: 0    // the default angle
    }
    states: [
        State {
            name: "flipped"
            PropertyChanges { target: rotation; angle: 360  }
        },
        State {
            name: "rotation"
            PropertyChanges { target: image_background; rotation: 360  }
        },
        State {
            name: "slidein"
            PropertyChanges { target: image_background; x: 0; y: 0 }
        },
        State {
            name: "slideout"
            PropertyChanges { target: image_background; x: 800; y: 0 }
        },
        State {
            name: "fadein"
            PropertyChanges { target: image_background; opacity: 1.0  }
        },
        State {
            name: "fadeout"
            PropertyChanges { target: image_background; opacity: 0.0  }
        }
    ]
    transitions: [
        Transition {
            from: "*"; to: "flipped"
            NumberAnimation { target: rotation; properties: "angle"; duration: currentDuration }
        },
        Transition {
            from: "*"; to: "rotation"
            NumberAnimation { properties: "rotation"; easing.type: Easing.InOutQuad; duration: currentDuration }
        },
        Transition {
            from: "*"; to: "slidein"
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce; duration: currentDuration }
        },
        Transition {
            from: "*"; to: "slideout"
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce; duration: currentDuration }
        },
        Transition {
            from: "*"; to: "fadein"
            NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: currentDuration }
        },
        Transition {
            from: "*"; to: "fadeout"
            NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: currentDuration }
        }
    ]
    Timer {
        interval: currentDuration
        running: true
        repeat: true
        onTriggered: {
            if (scene.state == '') {
                scene.currentDuration = 6000
                scene.state = 'flipped'
            } else if (scene.state == 'flipped') {
                scene.currentDuration = 2000
                scene.state = 'rotation'
            } else if (scene.state == 'rotation') {
                scene.currentDuration = 1000
                scene.state = 'slideout'
            } else if (scene.state == 'slideout') {
                scene.currentDuration = 1000
                scene.state = 'slidein'
            } else if (scene.state == 'slidein') {
                scene.currentDuration = 2000
                scene.state = 'fadeout'
            } else if (scene.state == 'fadeout') {
                scene.currentDuration = 2000
                scene.state = 'fadein'
            } else {
                scene.currentDuration = 200
                scene.state = ''
            }
        }
    }
}
*/
//テスト用(2)
/*
Rectangle {
    id: scene
    width: 800
    height: 480
    color: "#ff000000"
    property double ratioStatus: 0.25
    property int countFlash: 0
    Image { source: "image://image_provider/:/qmls/images/PXOSD_R0_Statusbar.png" }
    Image {
        id: image_status
        x: 0; y: 0
        width: 60 * ratioStatus; height: 60 * ratioStatus
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Status_AF.png"
    }
    states: [
        State {
            name: "fadein"
            PropertyChanges { target: image_status; opacity: 1.0  }
        },
        State {
            name: "fadeout"
            PropertyChanges { target: image_status; opacity: 0.0  }
        }
    ]
    transitions: [
        Transition {
            from: "*"; to: "fadein"
            NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
        },
        Transition {
            from: "*"; to: "fadeout"
            NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
        }
    ]
    Timer {
        interval: 1200
        running: true
        repeat: true
        onTriggered: {
            if (scene.state == 'fadein') {
                scene.state = 'fadeout'
                if (countFlash < 4) {
                    countFlash++
                } else {
                    countFlash = 0; ratioStatus = ratioStatus >= 8 ? 0.25 : ratioStatus*2
                }
            } else {
                scene.state = 'fadein'
            }
        }
    }
}
*/
