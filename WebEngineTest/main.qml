import QtQuick 2.10
import QtQuick.Window 2.10
import QtWebEngine 1.6

Window {
    id: _root
    visible: true
    width: 640
    height: 480
//    color: "transparent"
//    title: qsTr("Hello World")
//    flags: Qt.FramelessWindowHint | Qt.Window
    property string changecam: "changeModeCamera(\"09a1:0026\")"
    Component.onCompleted: {
//        _root.showFullScreen()
    }

    Rectangle{
        id: btn_test
//        anchors.top: browser.bottom
        anchors.bottom:  parent.bottom
        height: 50; width: 50
        color:"red"
        z: 1
        Text{text: "UPDATE"}
        MouseArea {
            anchors.fill: btn_test
            onPressed: {
                var js = '(document.getElementById("pen_red")).click()'
                browser.runJavaScript(js)
                // カメラ検出
                browser.runJavaScript("updateCameraList()")
            }
        }
    }
    Rectangle{
        id: btn_test2
        anchors.left: btn_test.right
        anchors.bottom:  parent.bottom
        height: 50; width: 50
        color:"yellow"
        z: 1
        Text{text: "720p"}
        MouseArea {
            anchors.fill: btn_test2
            onPressed: {
                var js = '(document.getElementById("pen_yellow")).click()'
                browser.runJavaScript(js)
                // カメラ設定
                browser.runJavaScript("setResolutionCamera(1280,720,30)")
                browser.runJavaScript(changecam, function(result){console.log(result)})
            }
        }
    }
    Rectangle{
        id: btn_test3
        anchors.left: btn_test2.right
        anchors.bottom:  parent.bottom
        height: 50; width: 50
        color:"skyblue"
        z: 1
        Text{text: "1080p"}
        MouseArea {
            anchors.fill: btn_test3
            onPressed: {
                var js = '(document.getElementById("pen_blue")).click()'
                browser.runJavaScript(js)
                //カメラ切替
                browser.runJavaScript("setResolutionCamera(1920,1080,30)")
                browser.runJavaScript(changecam, function(result){console.log(result)})
            }
        }
    }
    Rectangle{
        id: btn_test4
        anchors.left: btn_test3.right
        anchors.bottom:  parent.bottom
        height: 50; width: 50
        color:"white"
        z: 1
        Text{text: "0"}
        MouseArea {
            anchors.fill: parent
            onPressed: {
                var js = '(document.getElementById("pen_white")).click()'
                browser.runJavaScript(js)
                //カメラ切替
                browser.runJavaScript("changeModeCamera(\"\")", function(result){console.log(result)})
                browser.enabled = false
            }
        }
    }
    Rectangle{
        id: btn_test5
        anchors.left: btn_test4.right
        anchors.bottom:  parent.bottom
        height: 50; width: 50
        color:"red"
        z: 1
        Text{text: "VGA"}
        MouseArea {
            anchors.fill: parent
            onPressed: {
                var js = '(document.getElementById("pen_red")).click()'
                browser.runJavaScript(js)
                //カメラ切替
                browser.runJavaScript("setResolutionCamera(640,480,30)")
                browser.runJavaScript(changecam, function(result){console.log(result)})
            }
        }
    }
    // ブラウザ（描画領域）
    WebEngineView{
        id:browser
        anchors.fill: parent
        backgroundColor: "transparent"
        profile: WebEngineProfile{persistentCookiesPolicy: WebEngineProfile.NoPersistentCookies}
        z:0
//        url: "file:///page/J/Main.html"
//        url: "https://imagemate-c.com"
        url: "file:///opt/ELMO/EIT/page/J/Main.html"
        onFeaturePermissionRequested: {
            if(feature == WebEngineView.MediaAudioCapture ||
                    feature == WebEngineView.MediaVideoCapture ||
                    feature == WebEngineView.MediaAudioVideoCapture ||
                    feature == WebEngineView.Geolocation ||
                    feature == WebEngineView.DesktopVideoCapture ||
                    feature == WebEngineView.DesktopAudioVideoCapture){
                grantFeaturePermission(securityOrigin, feature, true)
            }
        }
    }
}
