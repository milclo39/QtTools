import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2

Rectangle {
    id: scene_setting_system_memory_play
    width: 800
    height: 480
    color: "#00000000" //透明

    //戻るボタン
    Button_Back{ filePath: "image://image_provider/:/qmls/images/PXOSD_R0_Back2"; component: componentSettingSystemMemory }
}
