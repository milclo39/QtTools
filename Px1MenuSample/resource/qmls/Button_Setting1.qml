import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

Rectangle {
    id: button_setting1
    width: 400
    height: 50

    property string filePath: "image://image_provider/:/qmls/images/blank_"
    property Component component: componentSetting

    Image {
        id: image_button_setting1
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        source: button_setting1.filePath + "N.png"
        MouseArea {
            anchors.fill: parent
            onPressed: {
                //console.log("onPressed >> Setting1 " + filePath)
                image_button_setting1.source = button_setting1.filePath + "P.png"
            }
            onContainsMouseChanged: {
                //console.log("onContainsMouseChanged >> Setting1 " + filePath)
                image_button_setting1.source = button_setting1.filePath + "N.png"
            }
            onCanceled: {
                //console.log("onCanceled >> Setting 1" + filePath)
                image_button_setting1.source = button_setting1.filePath + "N.png"
            }
            onReleased: {
                //console.log("onReleased >> Setting1 " + filePath)
                if (image_button_setting1.source == button_setting1.filePath + "P.png") {
                    image_button_setting1.source = button_setting1.filePath + "N.png"
                    stackView.push(component)
                }
            }
        }
    }
}
