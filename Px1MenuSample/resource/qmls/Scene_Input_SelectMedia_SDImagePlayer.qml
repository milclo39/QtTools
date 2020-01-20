import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick 2.0
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_input_selectmedia_sdimageplayer
    width: 800
    height: 480
    color: "#00000000" //透明

    property string datetime: ScreenMsgProc.datetime_play
    property string filename: ScreenMsgProc.filepath_play.replace("image://image_provider_thumbnail/","").replace(".JPG","")

    //背景
    Image {
        id: image_background
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_single_bg.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentInputSelectMediaSDImage }

    //ボタン
    Image {
        id: image_play
        x: 360; y: 390
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Play_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    image_play.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Play_P.png"
                }
            }
            onContainsMouseChanged: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    image_play.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Play_N.png"
                }
            }
            onReleased: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    if (image_play.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Play_P.png") {
                        image_play.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_V_Play_N.png"
                    }
                }
            }
        }
    }
    Image {
        id: image_del
        x: 734; y: 400
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    image_del.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_P.png"
                }
            }
            onContainsMouseChanged: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    image_del.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_N.png"
                }
            }
            onReleased: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    if (image_del.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_P.png") {
                        image_del.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_N.png"
                        root.clearMessage(); msg_08.visible = true; button_delete_yes.visible = true; button_delete_no.visible = true
                    }
                }
            }
        }
    }

    //日時
    Text { id: text_datetime; x: 40; y: 400; color: "white" ; font.pixelSize: 30; text: datetime }

    //ファイル名
    Text { id: text_filename; x: 40; y: 440; color: "white" ; font.pixelSize: 30; text: text_filename.x + text_filename.width > 350 ? filename.slice(0, 15) + "..." : filename }
}
