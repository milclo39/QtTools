import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import elmo.ScreenMsgProc 1.0

Rectangle {
    id: scene_input_selectmedia_usbimage
    width: 800
    height: 480
    color: "#00000000" //透明

    property int pageCurrent: 1
    property int pageMax: ScreenMsgProc.list_filepath.length % (row * column) == 0 ? ScreenMsgProc.list_filepath.length / (row * column) : ScreenMsgProc.list_filepath.length / (row * column) + 1

    property int row: 3
    property int column: 3

    property bool deleteMode: false

    //背景
    Image {
        id: image_background
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_bg.png"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentInputSelectMedia; visible: deleteMode == true ? false : true }

    //ボタン
    Image {
        id: image_r
        x: 24; y: 200
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_R_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    image_r.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_R_P.png"
                }
            }
            onContainsMouseChanged: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    image_r.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_R_N.png"
                }
            }
            onReleased: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    if (image_r.source == "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_R_P.png") {
                        image_r.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_R_N.png"
                        if (pageCurrent > 1) {
                            pageCurrent--
                        }
                    }
                }
            }
        }
    }
    Image {
        id: image_f
        x: 728; y: 200
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_F_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    image_f.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_F_P.png"
                }
            }
            onContainsMouseChanged: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    image_f.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_F_N.png"
                }
            }
            onReleased: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    if (image_f.source == "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_F_P.png") {
                        image_f.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_F_N.png"
                        if (pageCurrent < pageMax) {
                            pageCurrent++
                        }
                    }
                }
            }
        }
    }
    Image {
        id: image_del
        x: 728; y: 400
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
                    if (image_del.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_P.png") {
                        image_del.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_N.png"
                    }
                }
            }
            onReleased: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    if (image_del.source == "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_P.png") {
                        if (deleteMode == false) {
                            image_del.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_S.png"
                            deleteMode = true
                        } else {
                            image_del.source = "image://image_provider/:/qmls/images/PXOSD_R0_Memory_del_N.png"
                            root.clearMessage(); msg_08.visible = true; button_delete_yes.visible = true; button_delete_no.visible = true
                            deleteMode = false
                            stackView.pop(componentInputSelectMediaUSBImage)
                            stackView.push(componentInputSelectMediaUSBImage)
                        }
                    }
                }
            }
        }
    }
    Image {
        id: image_thumbnail
        x: 17; y: 0
        source: "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail9_N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    if (image_thumbnail.source == "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail9_N.png") {
                        image_thumbnail.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail9_P.png"
                    }
                    if (image_thumbnail.source == "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail4_N.png") {
                        image_thumbnail.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail4_P.png"
                    }
                }
            }
            onContainsMouseChanged: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    if (image_thumbnail.source == "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail9_P.png") {
                        image_thumbnail.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail9_N.png"
                    }
                    if (image_thumbnail.source == "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail4_P.png") {
                        image_thumbnail.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail4_N.png"
                    }
                }
            }
            onReleased: {
                if (msg_08.visible == false && msg_09.visible == false) {
                    if (image_thumbnail.source == "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail9_P.png") {
                        image_thumbnail.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail4_N.png"
                        row = 2; column = 2
                    }
                    if (image_thumbnail.source == "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail4_P.png") {
                        image_thumbnail.source = "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail9_N.png"
                        row = 3; column = 3
                    }
                }
            }
        }
    }

    //ページ
    Rectangle {
        x: 0; y: 450; width: 95
        Text { id: text_page; anchors.centerIn: parent; color: "white" ; font.pixelSize: 30; text: pageCurrent }
    }

    //サムネイル
    Grid {
        x: 101; y: 9 //枠考慮 デザイン指定=(x: 109; y: 15)
        rows: row; columns: column
        Repeater {
            model: row * column
            Rectangle {
                id: rect_select
                width: row == 3 ? 204 : 304
                height: row == 3 ? 154 : 229
                color: "#00000000"
                Image {
                    x: 3; y: 3
                    width: row == 3 ? 198 : 298
                    height: row == 3 ? 148 : 223
                    source: index + (pageCurrent - 1) * (row * column) < ScreenMsgProc.list_filepath.length ? ScreenMsgProc.list_filepath[index + (pageCurrent - 1) * (row * column)]
                                                                 : row == 3 ? "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_bg9.png"
                                                                            : "image://image_provider/:/qmls/images/PXOSD_R0_Thumbnail_bg4.png"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (msg_08.visible == false && msg_09.visible == false && index + (pageCurrent - 1) * (row * column) < ScreenMsgProc.list_filepath.length) {
                                ScreenMsgProc.filepath_play = ScreenMsgProc.list_filepath[index + (pageCurrent - 1) * (row * column)]
                                if (deleteMode == false) {
                                    ScreenMsgProc.type_req_datetime = ScreenMsgProc.filepath_play
                                    stackView.push(componentInputSelectMediaUSBImagePlayer)
                                } else if (rect_select.color == "#00000000") {
                                    ScreenMsgProc.list_filepath_delete.push(ScreenMsgProc.filepath_play)
                                    rect_select.color = define.colorBar
                                } else {
                                    ScreenMsgProc.list_filepath_delete.pop(ScreenMsgProc.filepath_play)
                                    rect_select.color = "#00000000"
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
