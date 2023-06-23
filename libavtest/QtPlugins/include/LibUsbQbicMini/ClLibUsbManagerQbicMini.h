//--------------------------------------------------------------------------------//
/*!
	@file	ClLibUsbManagerQbicMini.h
	@brief	QBiCMini用libusb通信管理クラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLLIBUSBMANAGERQBICMINI_H
#define CLLIBUSBMANAGERQBICMINI_H
#include "AbstLibUsbManager.h"

class ClLibUsbController;
class AbstCommandLibUsbQbicMini;

//--------------------------------------------------------------------------------//
/*!
	@brief	QBiCMini用libusb通信管理クラス
*/
//--------------------------------------------------------------------------------//
class ClLibUsbManagerQbicMini : public AbstLibUsbManager
{
public:
	ClLibUsbManagerQbicMini(QObject *pParent = NULL);	//コンストラクタ

protected:
	virtual QVector<QPair<quint16, quint16> > getSupportVendorProduct();	//対象ベンダ・プロダクトリスト取得
	int controlMessage(ClLibUsbController *pController, AbstCommandLibUsbQbicMini *pCommand, int timeout = 0);	//コントロール通信
	bool isGetCommonResult(quint16 usCmd);	//OK/NG結果取得のみコマンドか
	bool isGetString(quint16 usCmd);	//1文字列取得コマンドか
	bool isSetString(quint16 usCmd);	//1文字列設定コマンドか
	bool isSetNoParam(quint16 usCmd);	//パラメータなし設定コマンドか
	virtual bool isAddCommand(EL_CMD cmd);	//コマンド追加確認
	virtual void procCommand(EL_CMD cmd);	//コマンド処理
};
//--------------------------------------------------------------------------------//
#endif //CLLIBUSBMANAGERQBICMINI_H
