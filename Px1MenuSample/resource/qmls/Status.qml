import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: status
    x: 10
    y: 10
    width: 60
    height: 60
    color: "#00000000"
    opacity: 0.0
    state: ScreenMsgProc.state

    property int isVisible: 0
    property int index: 0

    property string filePath: ""

    property string fadein: "fadein_status"
    property string fadeout: "fadeout_status"

    function adjustPosition()
    {
        var index_new = 1
        var tmp_af = 0
        var tmp_freeze = 0
        var tmp_rotation = 0
        var tmp_rec = 0
        var tmp_cap = 0
        for (var i = 0; i < 5; i++) {
            if (status_af.isVisible == 1 && status_af.index == i) {
                tmp_af = index_new; status_af.y = (index_new - 1) * 60 + 10; index_new++
            }
            if (status_freeze.isVisible == 1 && status_freeze.index == i) {
                tmp_freeze = index_new; status_freeze.y = (index_new - 1) * 60 + 10; index_new++
            }
            if (status_rotation.isVisible == 1 && status_rotation.index == i) {
                tmp_rotation = index_new; status_rotation.y = (index_new - 1) * 60 + 10; index_new++
            }
            if (status_rec.isVisible == 1 && status_rec.index == i) {
                tmp_rec = index_new; status_rec.y = (index_new - 1) * 60 + 10; index_new++
            }
            if (status_cap.isVisible == 1 && status_cap.index == i) {
                tmp_cap = index_new; status_cap.y = (index_new - 1) * 60 + 10; index_new++
            }
        }
        status_af.index = tmp_af; status_freeze.index = tmp_freeze; status_rotation.index = tmp_rotation; status_rec.index = tmp_rec; status_cap.index = tmp_cap;

        if (fadein == "fadein_rotation") {
            if (ScreenMsgProc.current_rotation == 90) {
                visible = true; filePath = "image://image_provider/:/qmls/images/PXOSD_R0_Status_rotation090.png"
            } else if (ScreenMsgProc.current_rotation == 180) {
                visible = true; filePath = "image://image_provider/:/qmls/images/PXOSD_R0_Status_rotation180.png"
            } else if (ScreenMsgProc.current_rotation == 270) {
                visible = true; filePath = "image://image_provider/:/qmls/images/PXOSD_R0_Status_rotation270.png"
            } else {
                visible = false
            }
        }
    }

    Image { source: status.filePath }

    states: [
        State {
            name: fadein
            PropertyChanges { target: status; opacity: 1.0; restoreEntryValues: false }
        },
        State {
            name: fadeout
            PropertyChanges { target: status; opacity: 0.0; restoreEntryValues: false }
        }
    ]

    transitions: [
        Transition {
            from: "*"; to: fadein
            NumberAnimation {
                properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200
                onEasingChanged: {
                    isVisible = 1; adjustPosition()
                }
            }
        },
        Transition {
            from: "*"; to: fadeout
            NumberAnimation {
                properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200
                onEasingChanged: {
                    isVisible = 0; adjustPosition()
                }
            }
        }
    ]
}
