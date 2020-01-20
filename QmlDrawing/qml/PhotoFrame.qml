import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.0
import QtQuick.Window 2.1

Rectangle {
    id: photoFrame
    color: "transparent"
    width: image.width * image.scale + 20
    height: image.height * image.scale + 20
    border.color: "transparent"
    border.width: 2
    smooth: true
    antialiasing: true
    x: Math.random() * _root.width - defaultSize
    y: Math.random() * _root.height - defaultSize

    Image {
        id: image
        anchors.fill: parent
        fillMode: Image.Stretch
        source: folderModel.folder + fileName
        antialiasing: true
    }
    PinchArea {
        anchors.fill: parent
        pinch.target: photoFrame
        pinch.minimumRotation: -360
        pinch.maximumRotation: 360
        pinch.minimumScale: 0.1
        pinch.maximumScale: 10
        onPinchStarted: setFrameColor();
        MouseArea {
            id: dragArea
            hoverEnabled: true
            anchors.fill: parent
            drag.target: photoFrame
            onPressed: {
                photoFrame.z = ++_root.highestZ;
                parent.setFrameColor();
            }
            //onEntered: parent.setFrameColor();
            onWheel: {
                if (wheel.modifiers & Qt.ControlModifier) {
                    photoFrame.rotation += wheel.angleDelta.y / 120 * 5;
                    if (Math.abs(photoFrame.rotation) < 4)
                        photoFrame.rotation = 0;
                } else {
                    photoFrame.rotation += wheel.angleDelta.x / 120;
                    if (Math.abs(photoFrame.rotation) < 0.6)
                        photoFrame.rotation = 0;
                    var scaleBefore = image.scale;
                    image.scale += image.scale * wheel.angleDelta.y / 120 / 10;
                    photoFrame.x -= image.width * (image.scale - scaleBefore) / 2.0;
                    photoFrame.y -= image.height * (image.scale - scaleBefore) / 2.0;
                }
            }
        }
        function setFrameColor() {
            if (curImageFrame){
                curImageFrame.border.color = "transparent";
                curImageMimi.color = "transparent"
            }
            //選択されたフレームにフォーカス
            curImageFrame = photoFrame;
            curImageMimi = mimi;
            curImageFrame.border.color = "red";
            curImageMimi.color = "red"
        }
    }
    Rectangle {
        id: mimi
        anchors.bottom: photoFrame.bottom
        anchors.right: photoFrame.right
        width: 10
        height: 10
        color: "transparent"
        MouseArea {
            anchors.fill: parent
            //drag.target: photoFrame
            onMouseXChanged: {
                photoFrame.width += mouseX;
                photoFrame.height += mouseY;
            }
        }
    }
}
