import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

//展示会用ボタン
Rectangle {
    id: button_show
    width: 90
    height: 90
    radius: 45
    color: "black"
    border.color: "#ffdaad62"
    border.width: 2

    property string filePath: "image://image_provider/:/qmls/images/blank_"
    property Component component: componentSetting

    Image {
        id: image_button_show
        anchors.top: parent.top
        anchors.topMargin: 7
        anchors.left: parent.left
        anchors.leftMargin: 5
        width: 80
        height: 80
        source: button_show.filePath + "N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                image_button_show.source = button_show.filePath + "P.png"
            }
            onContainsMouseChanged: {
                image_button_show.source = button_show.filePath + "N.png"
            }
            onCanceled: {
                image_button_show.source = button_show.filePath + "N.png"
            }
            onReleased: {
                if (image_button_show.source == button_show.filePath + "P.png") {
                    image_button_show.source = button_show.filePath + "N.png"
                    stackView.push(component)
                }
            }
        }
    }
}
