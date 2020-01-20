TEMPLATE = app

QT += qml quick widgets

CONFIG += c++11

SOURCES += main.cpp \
           ClQmlApplicationEngine.cpp \
           ClScreenMsgProc.cpp

HEADERS += ClQmlApplicationEngine.h \
           ClScreenMsgProc.h

RESOURCES += resource/qml.qrc \
             #resource/resource_screen_e.qrc
             #resource/resource_screen_j.qrc
             #resource/resource_screen_d.qrc
             #resource/resource_screen_f.qrc
             #resource/resource_screen_a.qrc

OTHER_FILES += resource/qmls/main.qml
OTHER_FILES += resource/qmls/Scene.qml
OTHER_FILES += resource/qmls/Scene_Setting.qml \
               resource/qmls/Scene_Setting_System.qml \
               resource/qmls/Scene_Setting_Camera.qml \
               resource/qmls/Scene_Setting_Network.qml \
               resource/qmls/Scene_Setting_ProductInfo.qml
OTHER_FILES += resource/qmls/Scene_Setting_System_01_Resolution.qml \
               resource/qmls/Scene_Setting_System_02_AspectRatio.qml \
               resource/qmls/Scene_Setting_System_03_Flicker.qml \
               resource/qmls/Scene_Setting_System_04_USBMode.qml \
               resource/qmls/Scene_Setting_System_05_AudioSwitch.qml \
               resource/qmls/Scene_Setting_System_06_SoundVol.qml \
               resource/qmls/Scene_Setting_System_07_OpSoundVol.qml \
               resource/qmls/Scene_Setting_System_08_RecSize.qml \
               resource/qmls/Scene_Setting_System_09_RecQuality.qml \
               resource/qmls/Scene_Setting_System_10_RecCompression.qml \
               resource/qmls/Scene_Setting_System_11_RecMode.qml \
               resource/qmls/Scene_Setting_System_12_TimelapseInterval.qml \
               resource/qmls/Scene_Setting_System_13_Backlight.qml \
               resource/qmls/Scene_Setting_System_14_PowerSaving.qml \
               resource/qmls/Scene_Setting_System_15_Language.qml \
               resource/qmls/Scene_Setting_System_16_Initialize.qml \
               resource/qmls/Scene_Setting_System_17_SDFormat.qml \
               resource/qmls/Scene_Setting_System_18_USBFormat.qml \
               resource/qmls/Scene_Setting_System_19_Timeset.qml \
               resource/qmls/Scene_Setting_System_20_Rotation.qml \
               resource/qmls/Scene_Setting_System_21_Freeze.qml \
               resource/qmls/Scene_Setting_System_22_Memory.qml \
               resource/qmls/Scene_Setting_System_22_Memory_Play.qml \
               resource/qmls/Scene_Setting_System_23_HDCP.qml
OTHER_FILES += resource/qmls/Scene_Setting_Camera_01_AF.qml \
               resource/qmls/Scene_Setting_Camera_02_DZoom.qml \
               resource/qmls/Scene_Setting_Camera_03_WhiteBalance.qml \
               resource/qmls/Scene_Setting_Camera_04_ImageMode.qml \
               resource/qmls/Scene_Setting_Camera_05_Mask.qml \
               resource/qmls/Scene_Setting_Camera_06_ContourCorrect.qml \
               resource/qmls/Scene_Setting_Camera_07_Contrast.qml
OTHER_FILES += resource/qmls/Scene_Setting_ProductInfo_01_Camera.qml \
               resource/qmls/Scene_Setting_ProductInfo_02_Network.qml \
               resource/qmls/Scene_Setting_ProductInfo_03_License.qml
OTHER_FILES += resource/qmls/Scene_Debug.qml \
               resource/qmls/Scene_Debug_Grid.qml \
               resource/qmls/Scene_Debug_Touch.qml \
               resource/qmls/Scene_Debug_Syslog.qml \
               resource/qmls/Scene_Debug_Status.qml
OTHER_FILES += resource/qmls/Scene_Brightness.qml \
               resource/qmls/Scene_Input.qml \
               resource/qmls/Scene_Input_SelectMedia.qml \
               resource/qmls/Scene_Input_SelectMedia_SDImage.qml \
               resource/qmls/Scene_Input_SelectMedia_SDVideo.qml \
               resource/qmls/Scene_Input_SelectMedia_USBImage.qml \
               resource/qmls/Scene_Input_SelectMedia_SDImagePlayer.qml \
               resource/qmls/Scene_Input_SelectMedia_SDVideoPlayer.qml \
               resource/qmls/Scene_Input_SelectMedia_USBImagePlayer.qml
OTHER_FILES += resource/qmls/Button_Back.qml \
               resource/qmls/Button_Setting1.qml \
               resource/qmls/Button_Setting2.qml \
               resource/qmls/Button_Setting3.qml \
               resource/qmls/Button_Debug.qml \
               resource/qmls/Button_Show.qml
OTHER_FILES += resource/qmls/Status.qml
OTHER_FILES += resource/qmls/Zoombar.qml
OTHER_FILES += resource/qmls/Text_Number.qml
OTHER_FILES += resource/qmls/Define.qml \

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =
