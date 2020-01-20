/*
 * clScreenMsgProc.h
 *
 *  Created on: 2017/10/26
 *      Author: uchida
 */

#ifndef CLSCREENMSGPROC_H_
#define CLSCREENMSGPROC_H_

#include <QObject>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QQueue>
#include <QMutex>
#include <QTimer>
#include <QQuickItem>
#include <QStringList>

class ClScreenMsgProc : public QQuickItem
{
    Q_OBJECT

    //明るさ
    Q_PROPERTY(bool state_brightness_lamp READ get_B1_1 WRITE set_B1_1 NOTIFY changedSetting)
    Q_PROPERTY(int state_brightness_aperture READ get_B1_2 WRITE set_B1_2 NOTIFY changedSetting)
    Q_PROPERTY(bool state_brightness_antireflection READ get_B1_3 WRITE set_B1_3 NOTIFY changedSetting)
    //入力切替
    Q_PROPERTY(QString state_input READ get_I1_1 WRITE set_I1_1 NOTIFY changedSetting)
    Q_PROPERTY(bool state_input_hdcp READ get_I1_2 WRITE set_I1_2 NOTIFY changedSetting)
    Q_PROPERTY(QString state_input_sdvideo READ get_I1_3 WRITE set_I1_3 NOTIFY changedSetting)
    Q_PROPERTY(QStringList list_filepath READ get_I1_4 WRITE set_I1_4 NOTIFY changedSetting)
    Q_PROPERTY(QString filepath_play READ get_I1_5 WRITE set_I1_5 NOTIFY changedSetting)
    Q_PROPERTY(QString datetime_play READ get_I1_6 WRITE set_I1_6 NOTIFY changedSetting)
    Q_PROPERTY(QStringList list_filepath_delete READ get_I1_7 WRITE set_I1_7 NOTIFY changedSetting)
    Q_PROPERTY(QString type_req_file WRITE set_I1_type_req_file)
    Q_PROPERTY(QString type_req_datetime WRITE set_I1_type_req_datetime)
    Q_PROPERTY(QString video_control WRITE set_I1_video_control)
    Q_PROPERTY(bool delete_files WRITE set_I1_delete_files)
    //システム設定
    Q_PROPERTY(QString state_set2_S1_01 READ get_S1_01 WRITE set_S1_01 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_02 READ get_S1_02 WRITE set_S1_02 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_03 READ get_S1_03 WRITE set_S1_03 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_04 READ get_S1_04 WRITE set_S1_04 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_05 READ get_S1_05 WRITE set_S1_05 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_06 READ get_S1_06 WRITE set_S1_06 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_07 READ get_S1_07 WRITE set_S1_07 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_08 READ get_S1_08 WRITE set_S1_08 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_09 READ get_S1_09 WRITE set_S1_09 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_10 READ get_S1_10 WRITE set_S1_10 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_11 READ get_S1_11 WRITE set_S1_11 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_12 READ get_S1_12 WRITE set_S1_12 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_13 READ get_S1_13 WRITE set_S1_13 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_14 READ get_S1_14 WRITE set_S1_14 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_15 READ get_S1_15 WRITE set_S1_15 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_16 READ get_S1_16 WRITE set_S1_16 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_17 READ get_S1_17 WRITE set_S1_17 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_18 READ get_S1_18 WRITE set_S1_18 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_19 READ get_S1_19 WRITE set_S1_19 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_20 READ get_S1_20 WRITE set_S1_20 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_21 READ get_S1_21 WRITE set_S1_21 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_22 READ get_S1_22 WRITE set_S1_22 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_S1_23 READ get_S1_23 WRITE set_S1_23 NOTIFY changedSetting)
    //カメラ画質設定
    Q_PROPERTY(QString state_set2_C1_1 READ get_C1_1 WRITE set_C1_1 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_C1_2 READ get_C1_2 WRITE set_C1_2 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_C1_3 READ get_C1_3 WRITE set_C1_3 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_C1_4 READ get_C1_4 WRITE set_C1_4 NOTIFY changedSetting)
    Q_PROPERTY(QList<int> state_set2_C1_5 MEMBER m_state_set2_C1_5 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_C1_6 READ get_C1_6 WRITE set_C1_6 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_C1_7 READ get_C1_7 WRITE set_C1_7 NOTIFY changedSetting)
    //製品情報
    Q_PROPERTY(QString state_set2_P1_1 READ get_P1_1 WRITE set_P1_1 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_P1_2 READ get_P1_2 WRITE set_P1_2 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_P1_3 READ get_P1_3 WRITE set_P1_3 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_P1_3_main MEMBER m_state_set2_P1_3_main NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_P1_3_m0 MEMBER m_state_set2_P1_3_m0 NOTIFY changedSetting)
    Q_PROPERTY(QString state_set2_P1_3_fpga MEMBER m_state_set2_P1_3_fpga NOTIFY changedSetting)
    //デバッグ
    Q_PROPERTY(bool visible_debug MEMBER m_visible_debug NOTIFY changedSetting)
    Q_PROPERTY(QString text_debug_log MEMBER m_text_debug_log NOTIFY changedSetting)
    //ズーム
    Q_PROPERTY(int current_zuumbar MEMBER m_current_zuumbar NOTIFY changedSetting)
    //状態
    Q_PROPERTY(int current_rotation MEMBER m_current_rotation NOTIFY changedSetting)

public:
	explicit ClScreenMsgProc(QQuickItem *parent = 0);
	static ClScreenMsgProc *getInstance();

    static void registerElmoLibrary(){
        qmlRegisterSingletonType<ClScreenMsgProc>("elmo.ScreenMsgProc",1,0,"ScreenMsgProc", ClScreenMsgProc::qmlSingleton);
    }
    static QObject *qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine);

    //明るさ
    void set_B1_1(bool state_brightness_lamp) { m_state_brightness_lamp = state_brightness_lamp; }
    bool get_B1_1() const { return m_state_brightness_lamp; }
    void set_B1_2(int state_brightness_aperture);
    int get_B1_2() const { return m_state_brightness_aperture; }
    void set_B1_3(bool state_brightness_antireflection) { m_state_brightness_antireflection = state_brightness_antireflection; }
    bool get_B1_3() const { return m_state_brightness_antireflection; }
    //入力切替
    void set_I1_1(QString state_input) { m_state_input = state_input; }
    QString get_I1_1() const { return m_state_input; }
    void set_I1_2(bool state_input_hdcp) { m_state_input_hdcp = state_input_hdcp; }
    bool get_I1_2() const { return m_state_input_hdcp; }
    void set_I1_3(QString state_input_sdvideo) { m_state_input_sdvideo = state_input_sdvideo; }
    QString get_I1_3() const { return m_state_input_sdvideo; }
    void set_I1_4(QStringList list_filepath) { m_list_filepath = list_filepath; }
    QStringList get_I1_4() const { return m_list_filepath; }
    void set_I1_5(QString filepath_play) { m_filepath_play = filepath_play; }
    QString get_I1_5() const { return m_filepath_play; }
    void set_I1_6(QString datetime_play) { m_datetime_play = datetime_play; }
    QString get_I1_6() const { return m_datetime_play; }
    void set_I1_7(QStringList list_filepath_delete) { m_list_filepath_delete = list_filepath_delete; }
    QStringList get_I1_7() const { return m_list_filepath_delete; }
    void set_I1_type_req_file(QString type_req_file);
    void set_I1_type_req_datetime(QString type_req_datetime);
    void set_I1_video_control(QString video_control);
    void set_I1_delete_files(bool delete_files);
    //システム設定
    void set_S1_01(QString state_set2_S1_01) { m_state_set2_S1_01 = state_set2_S1_01; }
    QString get_S1_01() const { return m_state_set2_S1_01; }
    void set_S1_02(QString state_set2_S1_02) { m_state_set2_S1_02 = state_set2_S1_02; }
    QString get_S1_02() const { return m_state_set2_S1_02; }
    void set_S1_03(QString state_set2_S1_03) { m_state_set2_S1_03 = state_set2_S1_03; }
    QString get_S1_03() const { return m_state_set2_S1_03; }
    void set_S1_04(QString state_set2_S1_04) { m_state_set2_S1_04 = state_set2_S1_04; }
    QString get_S1_04() const { return m_state_set2_S1_04; }
    void set_S1_05(QString state_set2_S1_05) { m_state_set2_S1_05 = state_set2_S1_05; }
    QString get_S1_05() const { return m_state_set2_S1_05; }
    void set_S1_06(QString state_set2_S1_06) { m_state_set2_S1_06 = state_set2_S1_06; }
    QString get_S1_06() const { return m_state_set2_S1_06; }
    void set_S1_07(QString state_set2_S1_07) { m_state_set2_S1_07 = state_set2_S1_07; }
    QString get_S1_07() const { return m_state_set2_S1_07; }
    void set_S1_08(QString state_set2_S1_08) { m_state_set2_S1_08 = state_set2_S1_08; }
    QString get_S1_08() const { return m_state_set2_S1_08; }
    void set_S1_09(QString state_set2_S1_09) { m_state_set2_S1_09 = state_set2_S1_09; }
    QString get_S1_09() const { return m_state_set2_S1_09; }
    void set_S1_10(QString state_set2_S1_10) { m_state_set2_S1_10 = state_set2_S1_10; }
    QString get_S1_10() const { return m_state_set2_S1_10; }
    void set_S1_11(QString state_set2_S1_11) { m_state_set2_S1_11 = state_set2_S1_11; }
    QString get_S1_11() const { return m_state_set2_S1_11; }
    void set_S1_12(QString state_set2_S1_12) { m_state_set2_S1_12 = state_set2_S1_12; }
    QString get_S1_12() const { return m_state_set2_S1_12; }
    void set_S1_13(QString state_set2_S1_13) { m_state_set2_S1_13 = state_set2_S1_13; }
    QString get_S1_13() const { return m_state_set2_S1_13; }
    void set_S1_14(QString state_set2_S1_14) { m_state_set2_S1_14 = state_set2_S1_14; }
    QString get_S1_14() const { return m_state_set2_S1_14; }
    void set_S1_15(QString state_set2_S1_15);
    QString get_S1_15() const { return m_state_set2_S1_15; }
    void set_S1_16(QString state_set2_S1_16) { m_state_set2_S1_16 = state_set2_S1_16; }
    QString get_S1_16() const { return m_state_set2_S1_16; }
    void set_S1_17(QString state_set2_S1_17) { m_state_set2_S1_17 = state_set2_S1_17; }
    QString get_S1_17() const { return m_state_set2_S1_17; }
    void set_S1_18(QString state_set2_S1_18) { m_state_set2_S1_18 = state_set2_S1_18; }
    QString get_S1_18() const { return m_state_set2_S1_18; }
    void set_S1_19(QString state_set2_S1_19) { m_state_set2_S1_19 = state_set2_S1_19; }
    QString get_S1_19() const { return m_state_set2_S1_19; }
    void set_S1_20(QString state_set2_S1_20) { m_state_set2_S1_20 = state_set2_S1_20; }
    QString get_S1_20() const { return m_state_set2_S1_20; }
    void set_S1_21(QString state_set2_S1_21) { m_state_set2_S1_21 = state_set2_S1_21; }
    QString get_S1_21() const { return m_state_set2_S1_21; }
    void set_S1_22(QString state_set2_S1_22) { m_state_set2_S1_22 = state_set2_S1_22; }
    QString get_S1_22() const { return m_state_set2_S1_22; }
    void set_S1_23(QString state_set2_S1_23) { m_state_set2_S1_23 = state_set2_S1_23; }
    QString get_S1_23() const { return m_state_set2_S1_23; }
    //カメラ画質設定
    void set_C1_1(QString state_set2_C1_1) { m_state_set2_C1_1 = state_set2_C1_1; }
    QString get_C1_1() const { return m_state_set2_C1_1; }
    void set_C1_2(QString state_set2_C1_2) { m_state_set2_C1_2 = state_set2_C1_2; }
    QString get_C1_2() const { return m_state_set2_C1_2; }
    void set_C1_3(QString state_set2_C1_3) { m_state_set2_C1_3 = state_set2_C1_3; }
    QString get_C1_3() const { return m_state_set2_C1_3; }
    void set_C1_4(QString state_set2_C1_4) { m_state_set2_C1_4 = state_set2_C1_4; }
    QString get_C1_4() const { return m_state_set2_C1_4; }
    void set_C1_5(QList<int> state_set2_C1_5) { m_state_set2_C1_5 = state_set2_C1_5; }
    QList<int> get_C1_5() const { return m_state_set2_C1_5; }
    void set_C1_6(QString state_set2_C1_6) { m_state_set2_C1_6 = state_set2_C1_6; }
    QString get_C1_6() const { return m_state_set2_C1_6; }
    void set_C1_7(QString state_set2_C1_7) { m_state_set2_C1_7 = state_set2_C1_7; }
    QString get_C1_7() const { return m_state_set2_C1_7; }
    //製品情報
    void set_P1_1(QString state_set2_P1_1) { m_state_set2_P1_1 = state_set2_P1_1; }
    QString get_P1_1() const { return m_state_set2_P1_1; }
    void set_P1_2(QString state_set2_P1_2) { m_state_set2_P1_2 = state_set2_P1_2; }
    QString get_P1_2() const { return m_state_set2_P1_2; }
    void set_P1_3(QString state_set2_P1_3) { m_state_set2_P1_3 = state_set2_P1_3; }
    QString get_P1_3() const { return m_state_set2_P1_3; }

    //明るさ
    bool m_state_brightness_lamp;
    int m_state_brightness_aperture;
    bool m_state_brightness_antireflection;
    //入力切換
    QString m_state_input;
    bool m_state_input_hdcp;
    QString m_state_input_sdvideo;
    QStringList m_list_filepath;
    QString m_filepath_play;
    QString m_datetime_play;
    QStringList m_list_filepath_delete;
    QString m_type_req_file;
    QString m_type_req_datetime;
    QString m_video_control;
    bool m_delete_files;
    //システム設定
    QString m_state_set2_S1_01;
    QString m_state_set2_S1_02;
    QString m_state_set2_S1_03;
    QString m_state_set2_S1_04;
    QString m_state_set2_S1_05;
    QString m_state_set2_S1_06;
    QString m_state_set2_S1_07;
    QString m_state_set2_S1_08;
    QString m_state_set2_S1_09;
    QString m_state_set2_S1_10;
    QString m_state_set2_S1_11;
    QString m_state_set2_S1_12;
    QString m_state_set2_S1_13;
    QString m_state_set2_S1_14;
    QString m_state_set2_S1_15;
    QString m_state_set2_S1_16;
    QString m_state_set2_S1_17;
    QString m_state_set2_S1_18;
    QString m_state_set2_S1_19;
    QString m_state_set2_S1_20;
    QString m_state_set2_S1_21;
    QString m_state_set2_S1_22;
    QString m_state_set2_S1_23;
    //カメラ画質設定
    QString m_state_set2_C1_1;
    QString m_state_set2_C1_2;
    QString m_state_set2_C1_3;
    QString m_state_set2_C1_4;
    QList<int> m_state_set2_C1_5;
    QString m_state_set2_C1_6;
    QString m_state_set2_C1_7;
    //製品情報
    QString m_state_set2_P1_1;
    QString m_state_set2_P1_2;
    QString m_state_set2_P1_3;
    QString m_state_set2_P1_3_main;
    QString m_state_set2_P1_3_m0;
    QString m_state_set2_P1_3_fpga;
    //Debug
    bool m_visible_debug;
    QString m_text_debug_log;
    //ズームバー
    int m_current_zuumbar;
    //状態
    int m_current_rotation;

signals:
    void sigSendChangeLanguage(QString text);
    void sigSendListFilePath(QString type);
    void sigSendImageDateTime(QString datetime);

    void changedSetting();

public slots:
    void slotRecvListFilePath(QVariant list) { m_list_filepath = list.toStringList(); }
    void slotRecvImageDateTime(QVariant datetime) { m_datetime_play = datetime.toString(); }
};

#endif // CLSCREENMSGPROC_H_
