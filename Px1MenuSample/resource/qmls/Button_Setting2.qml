import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

Rectangle {
    id: button_setting2
    width: 680
    height: 50
    state: "00"

    property string filePath: "image://image_provider/:/qmls/images/blank_"
    property Component component: componentSetting

    function getSource()
    {
        if (button_setting2.state.toString() == "00" || button_setting2.state.toString() == "01" ||
            button_setting2.state.toString() == "02" || button_setting2.state.toString() == "03" ||
            button_setting2.state.toString() == "04" || button_setting2.state.toString() == "05" ||
            button_setting2.state.toString() == "06" || button_setting2.state.toString() == "07" ||
            button_setting2.state.toString() == "08" || button_setting2.state.toString() == "09" ||
            button_setting2.state.toString() == "10" ||
            button_setting2.state.toString() == "FHD" || button_setting2.state.toString() == "HD" ||
            button_setting2.state.toString() == "UHD" || button_setting2.state.toString() == "UXGA" ||
            button_setting2.state.toString() == "XGA" ||
            button_setting2.state.toString() == "H264" ||
            button_setting2.state.toString() == "HEVC" ||
            button_setting2.state.toString() == "Rotation90" || button_setting2.state.toString() == "Rotation180" ||
            button_setting2.state.toString() == "UVCUAC" )
        {
            return "images/PXOSD_R0_Set2State_all_" + button_setting2.state.toString() + ".png"
        }
        else if (button_setting2.state.toString() == "J" || button_setting2.state.toString() == "E" ||
            button_setting2.state.toString() == "D" || button_setting2.state.toString() == "F" ||
            button_setting2.state.toString() == "A" )
        {
            return "images/PXOSD_R0_Set2State_Language_" + button_setting2.state.toString() + ".png"
        }
        else if (button_setting2.state.toString() != "")
        {
            return "qrc:///" + button_setting2.state.toString() + ".png"
        }
        return ""
    }

    Image {
        id: image_button_setting2
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: button_setting2.filePath + "N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                //console.log("onPressed >> Setting2 " + filePath)
                image_button_setting2.source = button_setting2.filePath + "P.png"
            }
            onContainsMouseChanged: {
                //console.log("onContainsMouseChanged >> Setting2 " + filePath)
                image_button_setting2.source = button_setting2.filePath + "N.png"
            }
            onCanceled: {
                //console.log("onCanceled >> Setting2 " + filePath)
                image_button_setting2.source = button_setting2.filePath + "N.png"
            }
            onReleased: {
                //console.log("onReleased >> Setting2 " + filePath)
                if (image_button_setting2.source == button_setting2.filePath + "P.png") {
                    if (component == componentSettingSystemSDFormat || component == componentSettingSystemUSBFormat) {
                        root.clearMessage(); msg_10.visible = true
                    }
                    if (component == componentSettingCameraMask) {
                        text_debug.y = 10
                    } else {
                        text_debug.y = 70
                    }
                    image_button_setting2.source = button_setting2.filePath + "N.png"
                    stackView.push(component)
                }
            }
        }
    }

    Image {
        id: image_state
        x: 460 //680 - 220
        y: 1
        width: 220
        height: 48
        source: getSource()
    }
}
