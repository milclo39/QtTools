import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.3
import elmo.Worker 1.0
import "../screens" as Screens

Item {
    width: main.implicitWidth
    height: main.implicitHeight
    StackView {
        id: view
        anchors.fill: parent
        property variant items: [
            setting.createObject(),
            setting_system.createObject(),
            setting_quality.createObject()
        ]
        function pushScreen(index){
            view.push({item: view.items[index], destroyOnPop:false, immediate:true})
        }
        Component.onCompleted: {
            view.pushScreen(0)
        }
        // 設定TOP
        Component {
            id: setting
            Rectangle {
                // タイトルバー
                Rectangle{
                    color: "black"
                    Image {
                        id: toolbar
                        source: "../images/PXOSD_R0_J_Set1_title.png"
                    }
                    width: parent.width
                    height: toolbar.height
                    Image {
                        source: "../images/PXOSD_R0_Back1N.png"
                        x: 740
                        MouseArea {
                            anchors.fill: parent
                            onClicked: Qt.quit()
                        }
                    }
                }
                Button {
                    id: btn1
                    Image {
                        id: image1
                        source: "../images/PXOSD_R0_J_Set1_1N.png"
                    }
                    width: image1.width
                    height: image1.height
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 168
                    onClicked: {
                        view.pushScreen(1)
                    }
                }
                Button {
                    id: btn2
                    Image {
                        id: image2
                        source: "../images/PXOSD_R0_J_Set1_2N.png"
                    }
                    width: image2.width
                    height: image2.height
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 242
                    onClicked: {
                        view.pushScreen(2)
                    }
                }
            }
        }
        // システム設定
        Component {
            id: setting_system
            Rectangle {
                // タイトルバー
                Rectangle{
                    id: rect
                    color: "black"
                    Image {
                        id: toolbar
                        source: "../images/PXOSD_R0_J_Set2_S1_title.png"
                    }
                    width: parent.width
                    height: toolbar.height
                    Image {
                        source: "../images/PXOSD_R0_Back2N.png"
                        x: 740
                        MouseArea {
                            anchors.fill: parent
                            onClicked: view.pop({immediate:true})
                        }
                    }
                }
                // リストの配置
                ListView{
                    id: _list
                    y: 50
                    width: parent.width
                    height: parent.height - _list.y
                    model: _model
                    delegate: Item {
                        Button {
                            Image {
                                id: image1
                                source: _url
                                width: sourceSize.width
                                height: sourceSize.height
                            }
                            x: 40
                            y: 94 + _number * 74 - _list.y
                            width: image1.width
                            height: image1.height
                            onClicked: {
                                view.pushScreen(2)
                            }
                        }
                    }
                }
                ListModel{
                    id: _model
                    ListElement{ _number: 0; _url: "../images/PXOSD_R0_J_Set2_S1_01N.png" }
                    ListElement{ _number: 1; _url: "../images/PXOSD_R0_J_Set2_S1_02N.png" }
                    ListElement{ _number: 2; _url: "../images/PXOSD_R0_J_Set2_S1_03N.png" }
                    ListElement{ _number: 3; _url: "../images/PXOSD_R0_J_Set2_S1_04N.png" }
                    ListElement{ _number: 4; _url: "../images/PXOSD_R0_J_Set2_S1_05N.png" }
                }

            }
        }
        // カメラ画質設定
        Component {
            id: setting_quality
            Rectangle {
                Rectangle{
                    color: "black"
                    Image {
                        id: toolbar
                        source: "../images/PXOSD_R0_J_Set3_S2_Resolution_title.png"
                    }
                    width: parent.width
                    height: toolbar.height
                    Image {
                        source: "../images/PXOSD_R0_Back2N.png"
                        x: 740
                        MouseArea {
                            anchors.fill: parent
                            onClicked: view.pop({immediate:true})
                        }
                    }
                }
                Button {
                    Image {
                        id: image1
                        source: "../images/PXOSD_R0_J_Set3_S2_Resolution_1N.png"
                    }
                    width: image1.width
                    height: image1.height
                    x: 50
                    y: 168
                    onClicked: {
                        ElWorker.setRequest("resolution", "auto", "")
                        view.pop({immediate:true})
                    }
                }
                Button {
                    Image {
                        id: image2
                        source: "../images/PXOSD_R0_J_Set3_S2_Resolution_2N.png"
                    }
                    width: image2.width
                    height: image2.height
                    x: 420
                    y: 168
                    onClicked: {
                        ElWorker.setRequest("resolution", "uhd", "")
                        view.pop({immediate:true})
                    }
                }
            }
        }
    }
}
