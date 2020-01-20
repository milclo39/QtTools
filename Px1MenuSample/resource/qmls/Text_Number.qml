import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

Rectangle {
    id: text_number
    x: 0
    y: 0
    state: ''

    property string typeNumber: "year"
    property int digit: 1 //桁
    property int number: 0
    property int number_tmp: -1
    property bool forceUp: false
    property bool forceDown: false

    function getSource(num)
    {
        if (typeNumber == "month") {
            if (digit == 1) {
                if (num < 0) {
                    return "images/PXOSD_R0_TEXT2_1.png"
                } else if (num > 1) {
                    return "images/PXOSD_R0_TEXT2_0.png"
                }
            }
        } else if (typeNumber == "day") {
            if (digit == 1) {
                if (num < 0) {
                    return "images/PXOSD_R0_TEXT2_" + month_max1 + ".png"
                } else if (num > month_max1) {
                    return "images/PXOSD_R0_TEXT2_0.png"
                }
            }
        } else if (typeNumber == "hours") {
            if (digit == 1) {
                if (num < 0) {
                    return "images/PXOSD_R0_TEXT2_2.png"
                } else if (num > 2) {
                    return "images/PXOSD_R0_TEXT2_0.png"
                }
            }
        } else if (typeNumber == "minutes" || typeNumber == "seconds") {
            if (digit == 1) {
                if (num < 0) {
                    return "images/PXOSD_R0_TEXT2_5.png"
                } else if (num > 5) {
                    return "images/PXOSD_R0_TEXT2_0.png"
                }
            }
        }
        if (num < 0) {
            return "images/PXOSD_R0_TEXT2_9.png"
        } else if (num > 9) {
            return "images/PXOSD_R0_TEXT2_0.png"
        }
        return "images/PXOSD_R0_TEXT2_" + num + ".png"
    }

    Image {
        id: number_current
        x: 29 * (text_number.digit - 1)
        source: "images/PXOSD_R0_TEXT2_" + text_number.number + ".png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                state = 'stop'
                if (stackView.currentItem.objectName == "SceneSettingSystemTimeset") {
                    resetSelect()
                    if (typeNumber == "year") {
                        scene_setting_system_timeset.selectYear()
                    }
                    if (typeNumber == "month") {
                        scene_setting_system_timeset.selectMonth()
                    }
                    if (typeNumber == "day") {
                        scene_setting_system_timeset.selectDay()
                    }
                    if (typeNumber == "hours") {
                        scene_setting_system_timeset.selectHours()
                    }
                    if (typeNumber == "minutes") {
                        scene_setting_system_timeset.selectMinutes()
                    }
                    if (typeNumber == "seconds") {
                        scene_setting_system_timeset.selectSeconds()
                    }
                }
            }
        }
    }
    Image {
        id: number_over
        x: 29 * (text_number.digit - 1)
        source: "images/PXOSD_R0_TEXT2_0.png"
        visible: false
    }
    Image {
        id: number_under
        x: 29 * (text_number.digit - 1)
        source: "images/PXOSD_R0_TEXT2_0.png"
        visible: false
    }

    onNumberChanged: {
        if (state == '') {
            state = 'stop'
        } else if (forceUp == true) {
            state = 'up'
        } else if (forceDown == true) {
            state = 'down'
        } else if (number_tmp > number && number < 0) {
            state = 'down'
        } else {
            state = 'up'
        }
    }

    states: [
        State {
            name: "up"
            PropertyChanges { target: number_under; visible: true; source: getSource(number + 1) }
        },
        State {
            name: "down"
            PropertyChanges { target: number_over; visible: true; source: getSource(number - 1) }
        },
        State {
            name: "stop"
            PropertyChanges { target: number_over; visible: false; source: getSource(number - 1) }
            PropertyChanges { target: number_under; visible: false; source: getSource(number + 1) }
        }
    ]
    transitions: [
        Transition {
            from: "*"; to: "up"
            PropertyAction { target: number_current; property: "y"; value: 0 }
            PropertyAction { target: number_under; property: "y"; value: 61 }
            NumberAnimation { target: number_current; property: "y"; to: -61; duration: 100; easing.type: Easing.InOutQuad }
            NumberAnimation { target: number_under; property: "y"; to: 0; duration: 100; easing.type: Easing.InOutQuad }
            onRunningChanged: {
                if (state == 'up' && !running) {
                    number_current.source = getSource(number); state = 'stop'; number_tmp = number; forceUp = false
                }
            }
        },
        Transition {
            from: "*"; to: "down"
            PropertyAction { target: number_current; property: "y"; value: 0 }
            PropertyAction { target: number_over; property: "y"; value: -61 }
            NumberAnimation { target: number_current; property: "y"; to: 61; duration: 100; easing.type: Easing.InOutQuad }
            NumberAnimation { target: number_over; property: "y"; to: 0; duration: 100; easing.type: Easing.InOutQuad }
            onRunningChanged: {
                if (state == 'down' && !running) {
                    number_current.source = getSource(number); state = 'stop'; number_tmp = number; forceDown = false
                }
            }
        },
        Transition {
            from: "*"; to: "stop"
            NumberAnimation { target: number_current; property: "y"; to: 0; duration: 0 }
        },
        Transition {
            NumberAnimation { properties: "x,y"; duration: 100 }
        }
    ]
}
