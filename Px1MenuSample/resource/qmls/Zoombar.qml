import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: zoombar
    x: 750
    y: 0
    width: 50
    height: 480
    color: "#00000000"
    opacity: 0.0
    state: ScreenMsgProc.state

    property bool dzoom: false

    property int backParam: 0
    property int currentParam: 0

    property bool enableFlash: false

    Image {
        source: "images/PXOSD_R0_Zoom_background.png"
    }

    Image {
        id: image_zoombar
        width: 12
        height: 362
        x: 18
        y: 83
        source: "images/PXOSD_R0_Zoombar.png"
    }

    Rectangle {
        id: ellipse_bottom
        width: 10
        height: width
        x: image_zoombar.x + 1
        y: image_zoombar.y + image_zoombar.height - ellipse_bottom.width
        color: define.colorBar
        border.color:  define.colorBar
        border.width: 1
        radius: width*0.5
    }
    Rectangle {
        id: rect_bar
        width: 10
        height: 10 * zoombar.currentParam
        x: image_zoombar.x + 1
        y: ellipse_bottom.y + (ellipse_top.width / 2) - rect_bar.height
        color: define.colorBar
    }
    Rectangle {
        id: rect_bar_d
        x: image_zoombar.x + 1
        y: ellipse_bottom.y + (ellipse_top.width / 2) - rect_bar.height
        width: rect_bar.width
        height: rect_bar.height - 181
        color: (zoombar.dzoom == true && rect_bar_d.y < 264) ? define.colorDZoom : define.colorBar //264 = 83 + 181
    }
    Rectangle {
        id: ellipse_top
        width: ellipse_bottom.width
        height: ellipse_bottom.height
        x: image_zoombar.x + 1
        y: rect_bar.y  - (ellipse_top.width / 2)
        color: (zoombar.dzoom == true && ellipse_top.y < 264) ? define.colorDZoom : define.colorBar //264 = 83 + 181
        border.color: (zoombar.dzoom == true && ellipse_top.y < 264) ? define.colorDZoom : define.colorBar
        border.width: 1
        radius: width*0.5
    }
    states: [
        State {
            name: "fadein_zoom"
            PropertyChanges { target: zoombar; opacity: 1.0; restoreEntryValues: false }
        },
        State {
            name: "fadeout_zoom"
            PropertyChanges { target: zoombar; opacity: 0.0; restoreEntryValues: false }
        }
    ]

    transitions: [
        Transition {
            from: "*"; to: "fadein_zoom"
            NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
        },
        Transition {
            from: "*"; to: "fadeout_zoom"
            NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
        }
    ]
    Timer {
        interval: 1000
        running: opacity != 0.0 ? true : false
        repeat: true
        onTriggered: {
            if (zoombar.currentParam == zoombar.backParam && zoombar.enableFlash == true) {
                if (opacity != 0.0) {
                    zoombar.state = 'fadeout_zoom'
                }
            }
            zoombar.backParam = zoombar.currentParam
        }
    }
}
