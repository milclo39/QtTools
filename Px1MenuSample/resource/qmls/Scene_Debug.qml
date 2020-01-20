import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2

Rectangle {
    id: scene_debug
    width: 800
    height: 480
    color: "#ffffffff" //白

    //背景
    Image {
        id: image_background
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: "images/PXOSD_R0_Background1.png"
    }

    //タイトル
    Text {
        text: "Debug"
        x: 30
        y: 10
        font.pointSize: 24
        font.bold: true
        color: "white"
    }

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentDebug }

    Rectangle {
        x: 40
        y: 94
        width: 730
        height: (50 + 24) * 5

        Flickable {
            id: flick
            anchors.fill: parent
            contentWidth: 730
            contentHeight: (50 + 24) * 4 //ボタン数=4
            clip: true

            Grid {
                id: grid_button
                columns: 1
                spacing: 24
                Button_Debug{ text: "Grid"; component: componentDebugGrid; } //グリッド表示ボタン
                Button_Debug{ text: "Touch"; component: componentDebugTouch; } //タッチ確認ボタン
                Button_Debug{ text: "Syslog"; component: componentDebugSyslog; } //Syslog確認ボタン
                Button_Debug{ text: "Status"; component: componentDebugStatus; } //ステータス確認ボタン
            }

            ScrollBar.vertical: ScrollBar {
                active: true;
                onActiveChanged: {
                    if (!active) {
                      active = true;
                    }
                }
            }
        }
    }
}
