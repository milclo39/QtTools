import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import elmo.ScreenMsgProc 1.0

ApplicationWindow {
    id: root
    visible: true
    title: "PX-1 Menu Sample"
    minimumWidth: 800
    minimumHeight: 480
    maximumWidth: 800
    maximumHeight: 480

    //共通定義
    Define { id: define }

    //メッセージ
    function clearMessage()
    {
        msg_01.visible = false
        msg_02.visible = false
        msg_03.visible = false
        msg_04.visible = false
        msg_05.visible = false
        msg_06.visible = false
        msg_07.visible = false
        msg_08.visible = false
        msg_09.visible = false
        msg_10.visible = false
        msg_11.visible = false
        msg_12.visible = false
        msg_13.visible = false
        msg_14.visible = false
    }
    function clearDeleteButton()
    {
        button_delete_yes.visible = false
        button_delete_no.visible = false
    }

    //メイン
    property Component componentLive: Scene { id: scene }

    //明るさ画面
    property Component componentBrightness: Scene_Brightness { id: scene_brightness }

    //入力切換
    property Component componentInput: Scene_Input { id: scene_input }
    property Component componentInputSelectMedia: Scene_Input_SelectMedia { id: scene_input_selectmedia }
    property Component componentInputSelectMediaSDImage: Scene_Input_SelectMedia_SDImage { id: scene_input_selectmedia_sdimage }
    property Component componentInputSelectMediaSDVideo: Scene_Input_SelectMedia_SDVideo { id: scene_input_selectmedia_sdvideo }
    property Component componentInputSelectMediaUSBImage: Scene_Input_SelectMedia_USBImage { id: scene_input_selectmedia_usbimage }
    property Component componentInputSelectMediaSDImagePlayer: Scene_Input_SelectMedia_SDImagePlayer { id: scene_input_selectmedia_sdimageplayer }
    property Component componentInputSelectMediaSDVideoPlayer: Scene_Input_SelectMedia_SDVideoPlayer { id: scene_input_selectmedia_sdvideoplayer }
    property Component componentInputSelectMediaUSBImagePlayer: Scene_Input_SelectMedia_USBImagePlayer { id: scene_input_selectmedia_usbimageplayer }

    //設定画面
    property Component componentSetting: Scene_Setting { id: scene_setting; visibleDebug: ScreenMsgProc.visible_debug }
    property Component componentSettingSystem: Scene_Setting_System { id: scene_setting_system }
    property Component componentSettingCamera: Scene_Setting_Camera { id: scene_setting_camera }
    property Component componentSettingNetwork: Scene_Setting_Network { id: scene_setting_network }
    property Component componentSettingProductInfo: Scene_Setting_ProductInfo { id: scene_setting_productinfo }

    //システム設定画面
    property Component componentSettingSystemResolution: Scene_Setting_System_01_Resolution { id: scene_setting_system_resolution }
    property Component componentSettingSystemAspectRatio: Scene_Setting_System_02_AspectRatio { id: scene_setting_system_aspectratio }
    property Component componentSettingSystemFlicker: Scene_Setting_System_03_Flicker { id: scene_setting_system_flicker }
    property Component componentSettingSystemUSBMode: Scene_Setting_System_04_USBMode { id: scene_setting_system_usbmode }
    property Component componentSettingSystemAudioSwitch: Scene_Setting_System_05_AudioSwitch { id: scene_setting_system_audioswitch }
    property Component componentSettingSystemSoundVol: Scene_Setting_System_06_SoundVol { id: scene_setting_system_soundvol }
    property Component componentSettingSystemOpSoundVol: Scene_Setting_System_07_OpSoundVol { id: scene_setting_system_opsoundvol }
    //property Component componentSettingSystemRecSize: Scene_Setting_System_08_RecSize { id: scene_setting_system_recsize }
    property Component componentSettingSystemRecQuality: Scene_Setting_System_09_RecQuality { id: scene_setting_system_recquality }
    property Component componentSettingSystemRecCompression: Scene_Setting_System_10_RecCompression { id: scene_setting_system_reccmpression }
    property Component componentSettingSystemRecMode: Scene_Setting_System_11_RecMode { id: scene_setting_system_recmode }
    property Component componentSettingSystemTimelapseInterval: Scene_Setting_System_12_TimelapseInterval { id: scene_setting_system_timelapseinterval }
    property Component componentSettingSystemBacklight: Scene_Setting_System_13_Backlight { id: scene_setting_system_backlight }
    property Component componentSettingSystemPowerSaving: Scene_Setting_System_14_PowerSaving { id: scene_setting_system_powersaving }
    property Component componentSettingSystemLanguage: Scene_Setting_System_15_Language { id: scene_setting_system_language }
    property Component componentSettingSystemInitialize: Scene_Setting_System_16_Initialize { id: scene_setting_system_initialize }
    property Component componentSettingSystemSDFormat: Scene_Setting_System_17_SDFormat { id: scene_setting_system_sdformat }
    property Component componentSettingSystemUSBFormat: Scene_Setting_System_18_USBFormat { id: scene_setting_system_usbformat }
    property Component componentSettingSystemTimeset: Scene_Setting_System_19_Timeset { id: scene_setting_system_timeset }
    property Component componentSettingSystemRotation: Scene_Setting_System_20_Rotation { id: scene_setting_system_rotation }
    property Component componentSettingSystemFreeze: Scene_Setting_System_21_Freeze { id: scene_setting_system_freze }
    property Component componentSettingSystemMemory: Scene_Setting_System_22_Memory { id: scene_setting_system_memory }
    property Component componentSettingSystemMemoryPlay: Scene_Setting_System_22_Memory_Play { id: scene_setting_system_memory_play }
    property Component componentSettingSystemHDCP: Scene_Setting_System_23_HDCP { id: scene_setting_system_hdcp }

    //カメラ画質設定画面
    property Component componentSettingCameraAF: Scene_Setting_Camera_01_AF { id: scene_setting_camera_af }
    property Component componentSettingCameraDZoom: Scene_Setting_Camera_02_DZoom { id: scene_setting_camera_dzoom }
    property Component componentSettingCameraWhiteBalance: Scene_Setting_Camera_03_WhiteBalance { id: scene_setting_camera_whitebalance }
    property Component componentSettingCameraImageMode: Scene_Setting_Camera_04_ImageMode { id: scene_setting_camera_imagemode }
    property Component componentSettingCameraMask: Scene_Setting_Camera_05_Mask { id: scene_setting_camera_mask }
    property Component componentSettingCameraContourCorrect: Scene_Setting_Camera_06_ContourCorrect { id: scene_setting_camera_contourcorrect }
    property Component componentSettingCameraContrast: Scene_Setting_Camera_07_Contrast { id: scene_setting_camera_contrast }

    //ネットワーク設定

    //製品情報
    property Component componentSettingProductInfoCamera: Scene_Setting_ProductInfo_01_Camera { id: scene_setting_productinfo_camera }
    property Component componentSettingProductInfoNetwork: Scene_Setting_ProductInfo_02_Network { id: scene_setting_productinfo_network }
    property Component componentSettingProductInfoLicense: Scene_Setting_ProductInfo_03_License { id: scene_setting_productinfo_license }

    //Debug画面
    property Component componentDebug: Scene_Debug { }
    property Component componentDebugGrid: Scene_Debug_Grid { }
    property Component componentDebugTouch: Scene_Debug_Touch { }
    property Component componentDebugSyslog: Scene_Debug_Syslog { }
    property Component componentDebugStatus: Scene_Debug_Status { }

    //遷移
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: componentLive
        delegate: StackViewDelegate {
/* //画面遷移時のアニメーションはカクつくため無
            function transitionFinished(properties)
            {
                properties.exitItem.opacity = 1
            }

            pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "opacity"
                    from: 0
                    to: 1
                }
                PropertyAnimation {
                    target: exitItem
                    property: "opacity"
                    from: 1
                    to: 0
                }
            }
*/
        }
    }

    //状態表示
    //ボタンリスト
    Rectangle {
        id: status_bar
        x: 0
        y: 0
        width: 76
        height: 480
        color: "#00000000"
        state: status_af.state == 'fadein_af' || status_freeze.state == 'fadein_freeze' || status_rotation.state == 'fadein_rotation' || status_rec.state == 'fadein_rec' || status_cap.state == 'fadein_cap' ? 'fadein_statusbar' : 'fadeout_statusbar'
        visible: true
        Image { source: "image://image_provider/:/qmls/images/PXOSD_R0_Statusbar.png" }
        states: [
            State {
                name: "fadein_statusbar"
                PropertyChanges { target: status_bar; opacity: 1.0  }
            },
            State {
                name: "fadeout_statusbar"
                PropertyChanges { target: status_bar; opacity: 0.0 }
            }
        ]
        transitions: [
            Transition {
                from: "*"; to: "fadein_statusbar"
                NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
            },
            Transition {
                from: "*"; to: "fadeout_statusbar"
                NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
            }
        ]
    }

    ListView {
        id: grid_status
        Status {
            id: status_af; filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Status_AF.png"; fadein: "fadein_af"; fadeout: "fadeout_af"
        }
        Status {
            id: status_freeze; filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Status_freeze.png"; fadein: "fadein_freeze"; fadeout: "fadeout_freeze"
        }
        Status {
            id: status_rotation; filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Status_rotation090.png"; fadein: "fadein_rotation"; fadeout: "fadeout_rotation"
            Timer {
                interval: 2000; running: status_rotation.isVisible == 1 ? true : false; repeat: false
                onTriggered: { status_rotation.index = 0; status_rotation.state = 'fadeout_rotation' }
            }
        }
        Status {
            id: status_rec; filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Status_rec.png"; fadein: "fadein_rec"; fadeout: "fadeout_rec"
        }
        Status {
            id: status_cap; filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Status_capture.png"; fadein: "fadein_cap"; fadeout: "fadeout_cap"
            Timer {
                interval: 2000; running: status_cap.isVisible == 1 ? true : false; repeat: false
                onTriggered: { status_cap.index = 0; status_cap.state = 'fadeout_cap' }
            }
        }
    }

    //ズームバー
    Zoombar { id: zoombar; dzoom: ScreenMsgProc.state_set2_C1_2 == "ON" ? true : false; currentParam: ScreenMsgProc.current_zuumbar; enableFlash: true }

    //メッセージ
    Image { id: msg_01; source: visible == false ? "" : "image://image_provider/:/Message01.png"; visible: false } //SDカードが準備できていません
    Image { id: msg_02; source: visible == false ? "" : "image://image_provider/:/Message02.png"; visible: false } //SDカードの容量がいっぱいになりました録画を終了します
    Image { id: msg_03; source: visible == false ? "" : "image://image_provider/:/Message03.png"; visible: false } //SDカードまたはUSBメモリが準備できていません
    Image { id: msg_04; source: visible == false ? "" : "image://image_provider/:/Message04.png"; visible: false } //RGB入力映像出力中
    Image { id: msg_05; source: visible == false ? "" : "image://image_provider/:/Message05.png"; visible: false } //映像表示ができません
    Image { id: msg_06; source: visible == false ? "" : "image://image_provider/:/Message06.png"; visible: false } //ファイル削除に成功しました
    Image { id: msg_07; source: visible == false ? "" : "image://image_provider/:/Message07.png"; visible: false } //ファイル削除に失敗しました
    Image { id: msg_08; source: visible == false ? "" : "image://image_provider/:/Message08.png"; visible: false } //削除しますか？
    Image { id: msg_09; source: visible == false ? "" : "image://image_provider/:/Message09.png"; visible: false } //本当に削除しますか？
    Image { id: msg_10; source: visible == false ? "" : "image://image_provider/:/Message10.png"; visible: false } //フォーマットしますか
    Image { id: msg_11; source: visible == false ? "" : "image://image_provider/:/Message11.png"; visible: false } //本当にフォーマットしますか
    Image { id: msg_12; source: visible == false ? "" : "image://image_provider/:/Message12.png"; visible: false } //フォーマットが完了しました
    Image { id: msg_13; source: visible == false ? "" : "image://image_provider/:/Message13.png"; visible: false } //フォーマット中
    Image { id: msg_14; source: visible == false ? "" : "image://image_provider/:/Message14.png"; visible: false } //フォーマットに失敗しました

    //削除ボタン
    Button_Setting3 { id: button_delete_yes; x: 50; y: 242; filePath: "image://image_provider/:/Set3_S2_Initializ_Yes"; strCmd: "Delete"; strParam: "Yes"; visible: false }
    Button_Setting3 { id: button_delete_no; x: 420; y: 242; filePath: "image://image_provider/:/Set3_S2_Initializ_No"; strCmd: "Delete"; strParam: "No"; visible: false }

    //Debug文字列表示
    Text {
        id: text_debug
        x: 10
        y: 10
        font.pointSize: 12
        color: define.colorDebug
        text: ScreenMsgProc.text_debug_log
        visible: ScreenMsgProc.visible_debug
    }
}
