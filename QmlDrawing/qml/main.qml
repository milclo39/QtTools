import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.0
import QtQuick.Window 2.1
import QtWebEngine 1.6
import Qt.labs.folderlistmodel 1.0
import DrawingArea 1.0
import ObjectArea 1.0
import FreeLineArea 1.0
import ImageArea 1.0

Window{
    id: _root
    width: Screen.width - 10
    height: Screen.height - 10
    color: "transparent"
//    title: qsTr("Drawing!")
    visible: true
    flags:Qt.FramelessWindowHint

    //シグナル
    signal qmlSignal()

    //プロパティ
    property variant path: []
    property int highestZ: 0
    property real defaultSize: 100
    property var curImageFrame: undefined
    property var curImageMimi: undefined

    //カメラ映像
	/*CameraView {
        id: cameraUI
        anchors.fill: parent
        color: "transparent"
    }*/
    //描画領域
    Drawing {
        id: drawingarea
        anchors.fill: parent
    }
    WebEngineView{
        id:browser
        anchors.top: parent.top
        anchors.bottom: btn_test.top
        height: 550; width: 600

//        url: "file:///C:VCGit/eit_cloud/public/J/Main.html"
        url:"https://eit-plus-cloud.firebaseapp.com/J/Main.html"
        onFeaturePermissionRequested: {
            if(feature == MediaAudioCapture || feature == MediaVideoCapture || feature == MediaAudioVideoCapture){
                grantFeaturePermission(securityOrigin, feature, true)
            }
        }
    }

    // 図形オブジェクト
    Repeater {
        id: _object
        model: ListModel{}
        Rectangle {
            x: _posX;y: _posY
            width: 100; height: 100;
            color: "transparent"
            Objecting {
                anchors.fill: parent
            }
            MouseArea {
                anchors.fill: parent
                drag.target: parent
            }
        }
    }
    // 自由線
    Repeater {
        id: _freeline
        model: ListModel{}
        Rectangle {
            x: _posX;y: _posY
            width: 100; height: 100;
            color: "transparent"
            FreeLining {
                anchors.fill: parent
                linePixmap: drawingarea.linePixmap
            }
            MouseArea {
                anchors.fill: parent
                drag.target: parent
            }
        }
    }
    // スタンプ
    Repeater {
        id: _stamp
        model: ListModel{}
        Rectangle{
            x: _posX//Math.random() * _root.width - defaultSize
            y: _posY//Math.random() * _root.height - defaultSize
            width: 100
            height: 100
            Image {
                anchors.fill: parent
                fillMode: Image.Stretch
                source: _src
            }
            MouseArea {
                anchors.fill: parent
                drag.target: parent
            }
        }
    }
    // 付箋オブジェクト
    Repeater {
        id: _husen
        model: ListModel{}
        Rectangle {
            color: "lightyellow"
            x: _posX;y: _posY
            width: 200; height: 100;
            border.color: "gray"
            border.width: 2
            MouseArea {
                anchors.fill: parent
                drag.target: parent
            }
            TextEdit {
                x: 5; y: 10;
                width: parent.width
                font.pointSize: 14
                text: modelData
            }
        }
    }
    // ファイルダイアログ
    FileDialog {
        id: fileDialog
        title: "Choose a folder with some images"
        selectFolder: true
        onAccepted: folderModel.folder = fileUrl + "/"
    }
    // 画像挿入
    Repeater {
        id: _image_insert
        model: FolderListModel {
            id: folderModel
            objectName: "folderModel"
            showDirs: false
            nameFilters: ["*.png", "*.jpg", "*.gif"]
        }
        PhotoFrame {}
    }
//***********************************************************************************
    // サイドメニュー
    Image {
        id: sideMenu
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        source: "../resources/folder.png"
        MouseArea {
            anchors.fill: parent
            anchors.margins: -10
            onClicked: {
                fileDialog.open()
            }
        }
    }
    //メニュー
    Rectangle {
        id: menu
        x: 120; y: 100
        width: 120; height: 300
//        anchors.horizontalCenter: parent.horizontalCenter
        color: "lime"
        radius: 5
        Label {
            x: 10; y: 10
            id: label
            font.pixelSize: 18
            text: qsTr("Menu")
        }
        MouseArea {
            anchors.fill: parent
            drag.target: menu
        }
        Button {
            id: button1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: label.bottom
            anchors.topMargin: 10
            width: 100
            height: 60
            text: qsTr("〇")
            onClicked:{
                _object.model.append({"_posX": 300,"_posY": 300});
            }
        }
        Button {
            id: button2
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: button1.bottom
            anchors.topMargin: 10
            width: 100
            height: 60
            text: qsTr("Stamp")
            onClicked:{
                _stamp.model.append({"_src": drawingarea.imagePath,"_posX": 300,"_posY": 300});
            }
        }
        ComboBox {
            id: combo1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: button2.bottom
            anchors.topMargin: 5
            width: 100
            model: ["pt2", "pt4", "pt8", "pt16", "pt24"]
            onCurrentIndexChanged:{
                drawingarea.setNum(currentIndex)
                drawingarea.update()
            }
        }
        ComboBox {
            id: combo2
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: combo1.bottom
            anchors.topMargin: 5
            width: 100
            model: ["red", "green", "blue", "yellow", "pink"]
            onCurrentIndexChanged:{
                drawingarea.setColor(currentText)
                drawingarea.update()
            }
        }
        ComboBox {
            id: combo3
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: combo2.bottom
            anchors.topMargin: 5
            width: 100
            model: ["white", "trans", "cam", "picture", "movie"]
            onCurrentIndexChanged:{
                drawingarea.setMode(currentIndex)
                drawingarea.update()
                if(currentText == "trans"){
                    _root.color = "transparent"
                    cameraUI.state = "VideoPreview"
                }
                else if(currentText == "cam"){
                    _root.color = "transparent"
                    cameraUI.state = "VideoCapture"
                }
                else{
                    _root.color = "white"
                    cameraUI.state = "VideoPreview"
                }
            }
        }
        Button {
            id: quit
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: combo3.bottom
            anchors.topMargin: 10
            width: 100
            text: qsTr("quit")
            onClicked:{
                Qt.quit()
            }
        }
    }
    Component.onCompleted: {
//        _root.showFullScreen()
    }
}
