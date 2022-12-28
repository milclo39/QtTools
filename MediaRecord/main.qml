import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

Window {
    id: root
    title: "Media recorder"
    width: 640
    height: 480
    visible: true

    Component.onCompleted: {
        recorder.record()
        recorder.stop()
    }

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
    }

    Camera { id: camera }
    AudioInput { id: audioInput; muted: false }
    ImageCapture { id: imageCapture }
    MediaDevices { id: mediaDevices }

    CaptureSession {
        id: captureSession
        recorder: recorder
        audioInput: audioInput
        camera: camera
        imageCapture: imageCapture
        videoOutput: videoOutput
    }

    MediaRecorder {
        id: recorder
        onRecorderStateChanged:
            (state) => {
                console.log("RecordState:" + state)
            }
        onActualLocationChanged:
            (url) => {
                console.log("ActualLocation:" + url)
            }
        onErrorOccurred:
            {
            }
    }

    Column {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        Button {
            text: "Record"
            onClicked: recorder.record()
        }
        Button {
            id: stopButton
            text: "Stop"
            onClicked: recorder.stop()
        }
        Button {
            text: "setting"
            onClicked: {
                recorder.outputLocation = "file:///C:/Users/muchida/Desktop/testMovie.mp4"
                recorder.quality = MediaRecorder.VeryHighQuality
                recorder.mediaFormat.videoCodec = MediaFormat.H264
                recorder.mediaFormat.audioCodec = MediaFormat.AAC
                console.log(camera.cameraFormat.resolution)
            }
        }
    }
}
