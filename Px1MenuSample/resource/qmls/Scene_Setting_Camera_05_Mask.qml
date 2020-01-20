import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_setting_camera_mask
    width: 800
    height: 480
    color: "#00000000" //透明

    property var listRect: ScreenMsgProc.state_set2_C1_5

    property double base_x: 800 / 128
    property double base_y: 480 / 128

    property int pos_x: 0
    property int pos_y: 0

    function getX(x)
    {
        return Math.round((x * 128) / 800) * base_x
    }

    function getY(y)
    {
        return Math.round((y * 128) / 480) * base_y
    }

    //座標確認用
    MouseArea {
        anchors.fill: parent
        onPressed: {
            pos_x = getX(mouseX)
            pos_y = getY(mouseY)
            mask.x = pos_x
            mask.y = pos_y
        }
        onMouseXChanged: {
            var x = mouseX > pos_x ? pos_x : getX(mouseX)
            var w = mouseX > pos_x ? getX(mouseX - pos_x) : getX(pos_x - mouseX)
            mask.x = Math.round(x); mask.width = w >= 800 - base_x ? Math.round(800 - base_x) : Math.round(w)
        }
        onMouseYChanged: {
            var y = mouseY > pos_y ? pos_y : getY(mouseY)
            var h = mouseY > pos_y ? getY(mouseY - pos_y) : getY(pos_y - mouseY)
            mask.y = Math.round(y); mask.height = h >= 480 - base_y ? Math.round(480 - base_y) : Math.round(h)
        }
    }

    //矩形
    Rectangle {
        id: mask
        x: listRect[0] * base_x; y: listRect[1] * base_y; width: listRect[2] * base_x; height: listRect[3] * base_y
        color: "black"
        visible: width > 0 && height > 0 ? true : false

        Drag.active: dragArea.drag.active
        Drag.hotSpot.x: 10
        Drag.hotSpot.y: 10

        MouseArea {
            id: dragArea
            anchors.fill: parent
            drag.target: parent
            onMouseXChanged: {
                mask.x = Math.round(getX(mask.x))
            }
            onMouseYChanged: {
                mask.y = Math.round(getY(mask.y))
            }
        }
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingCamera }

    //クリアボタン
    Image {
        id: image_button_clear
        x: 740; y: 70; width: 60; height: 60
        visible: mask.visible
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_button_clear.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_P.png"
            }
            onContainsMouseChanged: {
                image_button_clear.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_N.png"
            }
            onReleased: {
                image_button_clear.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_N.png"
                mask.x = 0; mask.y = 0; mask.width = 0; mask.height = 0
                ScreenMsgProc.state_set2_C1_5 = [0, 0, 0, 0]
            }
        }
    }

    //確定ボタン
    Image {
        id: image_button_confirm
        x: 740; y: 140; width: 60; height: 60
        visible: mask.visible
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_R_N.png"
        rotation: 90 //90度回転
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_button_confirm.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_R_P.png"
            }
            onContainsMouseChanged: {
                image_button_confirm.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_R_N.png"
            }
            onReleased: {
                image_button_confirm.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_R_N.png"
                var w = mask.x < 0 ? mask.width + mask.x : getX(800) < mask.width + mask.x ? mask.width - (mask.width + mask.x - getX(800)) : mask.width
                var h = mask.y < 0 ? mask.height + mask.y : getY(480) < mask.height + mask.y ?  mask.height - (mask.height + mask.y - getY(480)) : mask.height
                var x = mask.x < 0 ? 0 : mask.x
                var y = mask.y < 0 ? 0 : mask.y
                ScreenMsgProc.state_set2_C1_5 = [Math.round(x / base_x), Math.round(y / base_y), Math.round(w / base_x), Math.round(h / base_y)]
            }
        }
    }
}
