/*
 * clScreenMsgProc.cpp
 *
 *  Created on: 2017/10/26
 *      Author: uchida
 */

#include "ClScreenMsgProc.h"
#include <QDebug>

ClScreenMsgProc* g_pclScreenMsgProc = NULL;

//--------------------------------------------------------------------------------//
/*!
	@brief	コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClScreenMsgProc::ClScreenMsgProc(QQuickItem *parent) : QQuickItem(parent)
{
	g_pclScreenMsgProc = this;

	//明るさ
	m_state_brightness_lamp = false;
	m_state_brightness_aperture = 9;
	m_state_brightness_antireflection = false;
	//入力切替
	m_state_input = "Camera";
	m_state_input_hdcp = false;
	m_state_input_sdvideo = "00";
	m_list_filepath.clear();
	m_filepath_play = "";
	m_datetime_play = "0000:00:00 00:00:00";
	m_list_filepath_delete.clear();
	//システム設定
	m_state_set2_S1_01 = "Auto";
	m_state_set2_S1_02 = "Normal";
	m_state_set2_S1_03 = "Auto";
	m_state_set2_S1_04 = "UVCUAC";
	m_state_set2_S1_05 = "Mic";
	m_state_set2_S1_06 = "05";
	m_state_set2_S1_07 = "03";
	m_state_set2_S1_08 = "Medium";
	m_state_set2_S1_09 = "High";
	m_state_set2_S1_10 = "H264";
	m_state_set2_S1_11 = "Normal";
	m_state_set2_S1_12 = "01h";
	m_state_set2_S1_13 = "OFF";
	m_state_set2_S1_14 = "OFF";
	m_state_set2_S1_15 = "J";
	m_state_set2_S1_16 = "";
	m_state_set2_S1_17 = "";
	m_state_set2_S1_18 = "";
	m_state_set2_S1_19 = "0000-00-00 00:00:00";
	m_state_set2_S1_20 = "Rotation180";
	m_state_set2_S1_21 = "Freeze2";
	m_state_set2_S1_22 = "";
	m_state_set2_S1_23 = "Compatible";
	//カメラ画質設定
	m_state_set2_C1_1 = "Zoomsync";
	m_state_set2_C1_2 = "ON";
	m_state_set2_C1_3 = "Auto";
	m_state_set2_C1_4 = "Standard";
	m_state_set2_C1_5 = {0, 0, 0, 0};
	m_state_set2_C1_6 = "05";
	m_state_set2_C1_7 = "05";
	//製品情報
	m_state_set2_P1_1 = "";
	m_state_set2_P1_2 = "";
	m_state_set2_P1_3 = "";
	m_state_set2_P1_3_main = "Main\tCPU\t01.00.00";
	m_state_set2_P1_3_m0 = "M0\tCPU\t01.00.00";
	m_state_set2_P1_3_fpga = "FPGA\tCPU\t01.00.00";
	//デバッグ
	m_visible_debug = false;
	m_text_debug_log = "DEBUG\nLOG";
	//ズーム
	m_current_zuumbar = 0;
	//状態
	m_current_rotation = 0; //0/90/180/270
}

//---------------------------------------------------------------------------------------//
/*!
	@brief	インスタンス生成
*/
//---------------------------------------------------------------------------------------//
ClScreenMsgProc *ClScreenMsgProc::getInstance()
{
	return g_pclScreenMsgProc;
}

//---------------------------------------------------------------------------------------//
/*!
    @brief  シングルトン用定義
*/
//---------------------------------------------------------------------------------------//
QObject *ClScreenMsgProc::qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new ClScreenMsgProc();
}

//---------------------------------------------------------------------------------------//
/*!
    @brief  入力切替
*/
//---------------------------------------------------------------------------------------//
void ClScreenMsgProc::set_I1_type_req_file(QString type_req_file)
{
    if (!type_req_file.isNull()) {
        emit sigSendListFilePath(type_req_file);
    }
}
void ClScreenMsgProc::set_I1_type_req_datetime(QString type_req_datetime)
{
    if (!type_req_datetime.isNull()) {
        emit sigSendImageDateTime(type_req_datetime);
    }
}
void ClScreenMsgProc::set_I1_video_control(QString video_control)
{
    qDebug() << video_control;
}
void ClScreenMsgProc::set_I1_delete_files(bool delete_files)
{
    if (delete_files) {
        qDebug() << m_list_filepath_delete;
    }
}
//---------------------------------------------------------------------------------------//
/*!
    @brief  明るさ
*/
//---------------------------------------------------------------------------------------//
void ClScreenMsgProc::set_B1_2(int state_brightness_aperture)
{
    m_state_brightness_aperture = state_brightness_aperture;
}
//---------------------------------------------------------------------------------------//
/*!
    @brief  システム設定
*/
//---------------------------------------------------------------------------------------//
void ClScreenMsgProc::set_S1_15(QString state_set2_S1_15)
{
    m_state_set2_S1_15 = state_set2_S1_15;
    if (!state_set2_S1_15.isNull()) {
        emit sigSendChangeLanguage(state_set2_S1_15);
    }
}

