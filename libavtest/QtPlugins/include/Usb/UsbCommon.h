//--------------------------------------------------------------------------------//
/*!
	@file	UsbCommon.h
	@brief	共有ヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef USBCOMMON_H
#define USBCOMMON_H
#include "CmnDefine.h"
#include "qglobal.h"
#include <QTime>

// スケジュール設定
typedef struct
{
	qint32	lWeekDay;
	bool	bIsEnable;
	QTime	clStartTime;
	QTime	clEndTime;
}stScheduleSetting;

//顔検知
typedef struct
{
	quint16	ulPosX;
	quint16 ulPosY;
	quint16 ulHeigh;
	quint16 ulWidth;
	quint16 ulPoint;
	quint16 ulDummy;
}stFaceParam;

//ベンダ・プロダクトID対
typedef struct
{
	quint16 usVendor;
	quint16 usProduct;
}stVendorProduct;

//アクセスポイント情報
typedef struct
{
	QString strSsid;
	QString strMacaddress;
	quint32 lChannel;
	qint32 lCodeType;
	qint32 lAuthorizationType;
	quint32 lRssi;
}stAccesspointInfo;

//wi-fi情報
typedef struct
{
	quint32 ulLinkState;
	stAccesspointInfo stApInfo;
}stWifiInfo;

typedef QMap<qint32, stScheduleSetting> ScheduleSettings;	//< QBIC_VALUE_WEEKDAY_*, stScheduleSetting >
Q_DECLARE_METATYPE(ScheduleSettings);

typedef QList<stAccesspointInfo> AccesspointList;
Q_DECLARE_METATYPE(AccesspointList);

typedef QList<stFaceParam> FacePosList;
Q_DECLARE_METATYPE(FacePosList);

bool operator==(stVendorProduct stInfo1, stVendorProduct stInfo2);	//構造体等値比較プロトタイプ宣言

class ClUsbController;
ClUsbController *getController(EL_CMD clCommand);	//コマンドからUSB通信クラス取得

QString getUsberrorString(quint16 usError);	//USBエラーを文字列変換
QString getUsbCommand(QUuid clCmd);	//コマンドIDを文字列変換
QString getQbicCommanderrorString(quint16 usError);	//コマンドエラーを文字列変換

#define USB_CONNECT_WAIT_TIME	3000	//!< 接続検出待ち時間

#endif
