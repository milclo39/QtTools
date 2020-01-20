import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: button_setting3
    width: 330
    height: 50

    property string filePath: "image://image_provider/:/qmls/images/blank_"
    property string strCmd: ""
    property string strParam: ""
    property string stateButton: "N"

    function setNormal()
    {
        image_button_setting3.source = button_setting3.filePath + "N.png"
    }

    function changeStateInput()
    {
        if (strCmd == "Input") {
            scene_input.setNormal()
            if (strParam == "Camera") {
                ScreenMsgProc.state_input = "Camera"
                if (ScreenMsgProc.state_input_hdcp == false) {
                    stackView.clear(); stackView.push(stackView.initialItem); text_debug.y = 10
                }
            } else if (strParam == "SelectMedia") {
                ScreenMsgProc.state_input = "SelectMedia"
                stackView.push(componentInputSelectMedia)
            } else if (strParam == "HDMI1" || strParam == "HDMI2") {
                ScreenMsgProc.state_input = strParam
                root.clearMessage(); msg_05.visible = ScreenMsgProc.state_input_hdcp
                if (ScreenMsgProc.state_input_hdcp == false) {
                    stackView.clear(); stackView.push(stackView.initialItem); text_debug.y = 10
                }
            } else if (strParam == "RGB") {
                ScreenMsgProc.state_input = "RGB"
                root.clearMessage(); msg_04.visible = true; text_debug.y = 10
            }
        }
        if (strCmd == "SelectMedia") {
            ScreenMsgProc.type_req_file = strParam
            if (strParam == "SDimage") {
                 stackView.push(componentInputSelectMediaSDImage); text_debug.y = 10
            } else if (strParam == "SDvideo") {
                stackView.push(componentInputSelectMediaSDVideo); text_debug.y = 10
            } else if (strParam == "USBimage") {
                stackView.push(componentInputSelectMediaUSBImage); text_debug.y = 10
            }
        }
    }

    function changeStateSettingSystem()
    {
        if (strCmd == "Resolution") {
            scene_setting_system_resolution.setNormal()
            if (strParam == "Auto") {
                ScreenMsgProc.state_set2_S1_01 = "Auto"
            } else if (strParam == "HD") {
                ScreenMsgProc.state_set2_S1_01 = "HD"
            } else if (strParam == "UHD") {
                ScreenMsgProc.state_set2_S1_01 = "UHD"
            } else if (strParam == "UXGA") {
                ScreenMsgProc.state_set2_S1_01 = "UXGA"
            } else if (strParam == "FHD") {
                ScreenMsgProc.state_set2_S1_01 = "FHD"
            } else if (strParam == "XGA") {
                ScreenMsgProc.state_set2_S1_01 = "XGA"
            }
        } else if (strCmd == "AspectRatio") {
            scene_setting_system_aspectratio.setNormal()
            if (strParam == "Normal") {
                ScreenMsgProc.state_set2_S1_02 = "Normal"
            } else if (strParam == "Full") {
                ScreenMsgProc.state_set2_S1_02 = "Full"
            }
        } else if (strCmd == "Flicker") {
            scene_setting_system_flicker.setNormal()
            if (strParam == "50Hz") {
                ScreenMsgProc.state_set2_S1_03 = "50Hz"
            } else if (strParam == "60Hz") {
                ScreenMsgProc.state_set2_S1_03 = "60Hz"
            } else if (strParam == "Auto") {
                ScreenMsgProc.state_set2_S1_03 = "Auto"
            }
        } else if (strCmd == "USBMode") {
            scene_setting_system_usbmode.setNormal()
            if (strParam == "UVCUAC") {
                ScreenMsgProc.state_set2_S1_04 = "UVCUAC"
            } else if (strParam == "MassStorage") {
                ScreenMsgProc.state_set2_S1_04 = "MassStorage"
            }
        } else if (strCmd == "AudioSwitch") {
            scene_setting_system_audioswitch.setNormal()
            if (strParam == "Mic") {
                ScreenMsgProc.state_set2_S1_05 = "Mic"
            } else if (strParam == "HDMIaudio") {
                ScreenMsgProc.state_set2_S1_05 = "HDMIaudio"
            } else if (strParam == "AudioOff") {
                ScreenMsgProc.state_set2_S1_05 = "AudioOff"
            }
        } else if (strCmd == "RecSize") {
            scene_setting_system_recsize.setNormal()
            if (strParam == "High") {
                ScreenMsgProc.state_set2_S1_08 = "High"
            } else if (strParam == "Medium") {
                ScreenMsgProc.state_set2_S1_08 = "Medium"
            } else if (strParam == "Low") {
                ScreenMsgProc.state_set2_S1_08 = "Low"
            }
        } else if (strCmd == "RecQuality") {
            scene_setting_system_recquality.setNormal()
/*
            if (strParam == "High") {
                ScreenMsgProc.state_set2_S1_09 = "High"
            } else if (strParam == "Medium") {
                ScreenMsgProc.state_set2_S1_09 = "Medium"
            } else if (strParam == "Low") {
                ScreenMsgProc.state_set2_S1_09 = "Low"
            }
*/
            if (strParam == "SuperFine") {
                ScreenMsgProc.state_set2_S1_09 = "SuperFine"
            } else if (strParam == "Fine") {
                ScreenMsgProc.state_set2_S1_09 = "Fine"
            } else if (strParam == "Standard") {
                ScreenMsgProc.state_set2_S1_09 = "Standard"
            } else if (strParam == "Basic") {
                ScreenMsgProc.state_set2_S1_09 = "Basic"
            } else if (strParam == "Economy") {
                ScreenMsgProc.state_set2_S1_09 = "Economy"
            }
        } else if (strCmd == "RecCompression") {
            scene_setting_system_reccompression.setNormal()
            if (strParam == "H264") {
                ScreenMsgProc.state_set2_S1_10 = "H264"
            } else if (strParam == "HEVC") {
                ScreenMsgProc.state_set2_S1_10 = "HEVC"
            }
        } else if (strCmd == "RecMode") {
            scene_setting_system_recmode.setNormal()
            if (strParam == "Normal") {
                ScreenMsgProc.state_set2_S1_11 = "Normal"
            } else if (strParam == "TimelapseRec") {
                ScreenMsgProc.state_set2_S1_11 = "TimelapseRec"
            }
        } else if (strCmd == "TimelapseInterval") {
            scene_setting_system_timelapseinterval.setNormal()
            if (strParam == "01m") {
                ScreenMsgProc.state_set2_S1_12 = "01m"
            } else if (strParam == "10m") {
                ScreenMsgProc.state_set2_S1_12 = "10m"
            } else if (strParam == "30m") {
                ScreenMsgProc.state_set2_S1_12 = "30m"
            } else if (strParam == "01h") {
                ScreenMsgProc.state_set2_S1_12 = "01h"
            } else if (strParam == "03h") {
                ScreenMsgProc.state_set2_S1_12 = "03h"
            } else if (strParam == "06h") {
                ScreenMsgProc.state_set2_S1_12 = "06h"
            }
        } else if (strCmd == "Backlight") {
            scene_setting_system_backlight.setNormal()
            if (strParam == "OFF") {
                ScreenMsgProc.state_set2_S1_13 = "OFF"
            } else if (strParam == "30s") {
                ScreenMsgProc.state_set2_S1_13 = "30s"
            } else if (strParam == "01m") {
                ScreenMsgProc.state_set2_S1_13 = "01m"
            } else if (strParam == "05m") {
                ScreenMsgProc.state_set2_S1_13 = "05m"
            } else if (strParam == "10m") {
                ScreenMsgProc.state_set2_S1_13 = "10m"
            }
        } else if (strCmd == "PowerSaving") {
            scene_setting_system_powersaving.setNormal()
            if (strParam == "OFF") {
                ScreenMsgProc.state_set2_S1_14 = "OFF"
            } else if (strParam == "30m") {
                ScreenMsgProc.state_set2_S1_14 = "30m"
            } else if (strParam == "01h") {
                ScreenMsgProc.state_set2_S1_14 = "01h"
            } else if (strParam == "02h") {
                ScreenMsgProc.state_set2_S1_14 = "02h"
            } else if (strParam == "03h") {
                ScreenMsgProc.state_set2_S1_14 = "03h"
            }
        } else if (strCmd == "Language") {
            stackView.clear(); stackView.push(stackView.initialItem); text_debug.y = 10
            scene_setting_system_language.setNormal()
            if (strParam == "J") {
                ScreenMsgProc.state_set2_S1_15 = "J"
            } else if (strParam == "E") {
                ScreenMsgProc.state_set2_S1_15 = "E"
            } else if (strParam == "D") {
                ScreenMsgProc.state_set2_S1_15 = "D"
            } else if (strParam == "F") {
                ScreenMsgProc.state_set2_S1_15 = "F"
            } else if (strParam == "A") {
                ScreenMsgProc.state_set2_S1_15 = "A"
            }
        } else if (strCmd == "Initialize") {
            if (strParam == "No") {
                stackView.pop(componentSettingSystemInitialize)
            } else {
                ScreenMsgProc.state_set2_S1_16 = true
            }
        } else if (strCmd == "SDFormat" || strCmd == "USBFormat") {
            if (strParam == "Yes") {
                if (msg_10.visible == true) {
                    root.clearMessage(); msg_11.visible = true
                } else if (msg_11.visible == true) {
                    root.clearMessage(); msg_12.visible = true
                    if (strCmd == "SDFormat") {
                        ScreenMsgProc.state_set2_S1_17 = true
                        scene_setting_system_sdformat.visibleButton = false
                    } else if (strCmd == "USBFormat") {
                        ScreenMsgProc.state_set2_S1_18 = true
                        scene_setting_system_usbformat.visibleButton = false
                    }
                }
            } else if (strParam == "No") {
                root.clearMessage()
                if (strCmd == "SDFormat") {
                    stackView.pop(componentSettingSystemSDFormat)
                } else if (strCmd == "USBFormat") {
                    stackView.pop(componentSettingSystemUSBFormat)
                }
            }
        } else if (strCmd == "Timeset") {
            scene_setting_system_timeset.resetSelect();
            if (strParam == "OK") {
                ScreenMsgProc.state_set2_S1_19 =
                        image_year1.number.toString() + image_year2.number.toString() + image_year3.number.toString() + image_year4.number.toString() + "-" +
                        image_month1.number.toString() + image_month2.number.toString() + "-" +
                        image_day1.number.toString() + image_day2.number.toString() + " " +
                        image_hours1.number.toString() + image_hours2.number.toString() + ":" +
                        image_minutes1.number.toString() + image_minutes2.number.toString() + ":" +
                        image_seconds1.number.toString() + image_seconds2.number.toString()
            } else if (strParam == "Cancel") {
                stackView.pop(componentSettingSystemTimeset)
            }
        } else if (strCmd == "Rotation") {
            scene_setting_system_rotation.setNormal()
            if (strParam == "Rotation90") {
                ScreenMsgProc.state_set2_S1_20 = "Rotation90"
            } else if (strParam == "Rotation180") {
                ScreenMsgProc.state_set2_S1_20 = "Rotation180"
            }
        } else if (strCmd == "Freeze") {
            scene_setting_system_freeze.setNormal()
            if (strParam == "Freeze1") {
                ScreenMsgProc.state_set2_S1_21 = "Freeze1"
            } else if (strParam == "Freeze2") {
                ScreenMsgProc.state_set2_S1_21 = "Freeze2"
            }
        } else if (strCmd == "Memory") {
            if (strParam == "Play") {
                stackView.push(componentSettingSystemMemoryPlay); text_debug.y = 10
            }
        } else if (strCmd == "HDCP") {
            scene_setting_system_hdcp.setNormal()
            if (strParam == "Compatible") {
                ScreenMsgProc.state_set2_S1_23 = "Compatible"
            } else if (strParam == "Incompatible") {
                ScreenMsgProc.state_set2_S1_23 = "Incompatible"
            }
        }
    }

    function changeStateSettingCamera()
    {
        if (strCmd == "AF") {
            scene_setting_camera_af.setNormal()
            if (strParam == "Zoom sync") {
                ScreenMsgProc.state_set2_C1_1 = "Zoomsync"
            } else if (strParam == "One-push") {
                ScreenMsgProc.state_set2_C1_1 = "Onepush"
            } else if (strParam == "Continuous") {
                ScreenMsgProc.state_set2_C1_1 = "Continuous"
            }
        } else if (strCmd == "DZoom") {
            scene_setting_camera_dzoom.setNormal()
            if (strParam == "ON") {
                ScreenMsgProc.state_set2_C1_2 = "ON"
            } else if (strParam == "OFF") {
                ScreenMsgProc.state_set2_C1_2 = "OFF"
            }
        } else if (strCmd == "WhiteBalance") {
            scene_setting_camera_whitebalance.setNormal()
            if (strParam == "Auto") {
                ScreenMsgProc.state_set2_C1_3 = "Auto"
            } else if (strParam == "One-push") {
                ScreenMsgProc.state_set2_C1_3 = "Onepush"
            } else if (strParam == "Flourescent") {
                ScreenMsgProc.state_set2_C1_3 = "FluorescentLamp"
            } else if (strParam == "Indoors") {
                ScreenMsgProc.state_set2_C1_3 = "Indoor"
            } else if (strParam == "Outdoors") {
                ScreenMsgProc.state_set2_C1_3 = "Outdoor"
            }
        } else if (strCmd == "ImageMode") {
            scene_setting_camera_imagemode.setNormal()
            if (strParam == "Standard") {
                ScreenMsgProc.state_set2_C1_4 = "Standard"
            } else if (strParam == "Landscape") {
                ScreenMsgProc.state_set2_C1_4 = "Landscape"
            } else if (strParam == "Black/white") {
                ScreenMsgProc.state_set2_C1_4 = "WB"
            } else if (strParam == "Color emphasis") {
                ScreenMsgProc.state_set2_C1_4 = "ColorEmphasis"
            } else if (strParam == "DLP") {
                ScreenMsgProc.state_set2_C1_4 = "DLP"
            }
        } else {
            // Mask / ContourCorrect / Contrast は無
        }
    }

    Image {
        id: image_button_setting3
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: button_setting3.filePath + button_setting3.stateButton +".png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                //console.log("onPressed >> Setting3 " + filePath)
                if (image_button_setting3.source != button_setting3.filePath + "S.png") {
                    image_button_setting3.source = button_setting3.filePath + "P.png"
                }
            }
            onContainsMouseChanged: {
                //console.log("onContainsMouseChanged >> Setting3 " + filePath)
                if (image_button_setting3.source == button_setting3.filePath + "P.png") {
                    image_button_setting3.source = button_setting3.filePath + "N.png"
                }
            }
            onCanceled: {
                //console.log("onCanceled >> Setting3 " + filePath)
                if (image_button_setting3.source != button_setting3.filePath + "S.png") {
                    image_button_setting3.source = button_setting3.filePath + "N.png"
                }
            }
            onReleased: {
                //console.log("onReleased >> Setting3 " + filePath)
                if (image_button_setting3.source == button_setting3.filePath + "P.png") {
                    //ステータス更新
                    changeStateInput()              //入力切換
                    changeStateSettingSystem()      //システム設定
                    changeStateSettingCamera()      //カメラ画質設定
                    if (strCmd == "Delete") {
                        if (strParam == "Yes") {
                            if (msg_08.visible == true) {
                                root.clearMessage(); msg_09.visible = true
                            } else if (msg_09.visible == true) {
                                ScreenMsgProc.delete_files = true //削除実行
                                root.clearMessage(); clearDeleteButton()
                            }
                        } else if (strParam == "No") {
                            root.clearMessage(); clearDeleteButton()
                        }
                    }
                    //ボタン画像変更
                    if (strCmd == "SelectMedia" || strCmd == "Initialize" || strCmd == "SDFormat" || strCmd == "USBFormat" || strCmd == "Timeset" || strCmd == "Memory" || strCmd == "Delete") {
                        image_button_setting3.source = button_setting3.filePath + "N.png"
                    } else {
                        image_button_setting3.source = button_setting3.filePath + "S.png"
                    }
                }
            }
        }
    }
}
