#define __USB_DEBUG

#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/ClUsbDriver_win.h"
#include "win32/ClDeviceInfo_win.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "win32/ClPresenterMO.h"
#include "RollOverToolBtn/ClRollOverToolBtn.h"

#include <strmif.h>
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

// リコーUSB用エクステンションユニット識別用GUID
#include <initguid.h>
DEFINE_GUID(GUID_877_EXTENSION_UNIT, 0x74183e0a,0x5482,0x481a,0xb4,0x02,0x48,0xb8,0xb8,0xc4,0x9c,0xc8);

// 怪しげなXmlに設定するデバイス名 変更：L-12iのUVC/UACモードにも対応するため、一般的な名前にする
#define MO_PRODUCT_NAME	("ELMO UVC/UAC Camera")

// パラメータ
#define PARAM_MO1_ZOOM_CMD_SEND_RES				( 10 )				//!< ズームコマンドのパラメータの間隔
#define PARAM_MO1_FOCUS_CMD_SEND_RES			( 10 )				//!< フォーカスコマンドのパラメータの間隔
#define PARAM_MO1_BRIGHTNESS_CMD_SEND_RES		( 1 )				//!< 明度コマンドのパラメータの間隔

// 文言
#ifdef EU_MO1
#define WORD_MO1_REMOCON_LED		( ClPresenterMO::tr("LED") )					//!< リモコン 文言　LED
#define WORD_MO1_REMOCON_ROTATE		( ClPresenterMO::tr("Rotate") )					//!< リモコン 文言　Rotate
#else
#define WORD_MO1_REMOCON_LED		( "EU1" )										//!< リモコン 文言　Extension Unit 1
#define WORD_MO1_REMOCON_ROTATE		( "EU2" )										//!< リモコン 文言　Extension Unit 2
#endif

#define WORD_MO1_REMOCON_FLICK		( ClPresenterMO::tr("Flickerless") )			//!< リモコン 文言　Flicker

#define WORD_MO1_REMOCON_FLICK_DISABLE	( ClPresenterMO::tr("Off") )				//!< リモコン 文言　Flicker 無効
#define WORD_MO1_REMOCON_FLICK_50HZ		( ClPresenterMO::tr("50Hz") )				//!< リモコン 文言　Flicker 50Hz
#define WORD_MO1_REMOCON_FLICK_60HZ		( ClPresenterMO::tr("60Hz") )				//!< リモコン 文言　Flicker 60Hz

// 式
#define EXP_MO1_REMOCON_SET_ITEMLIST(clStrList)	( clStrList	<< WORD_MO1_REMOCON_FLICK_DISABLE	\
															<< WORD_MO1_REMOCON_FLICK_50HZ		\
															<< WORD_MO1_REMOCON_FLICK_60HZ		)	//!< フリッカ補正UI用の文字列をセット

#ifndef __DEBUG__
//#define __DEBUG__
#endif


//--------------------------------------------------------------------------------
/*!
	@fn			ClPresenterMO
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterMO::ClPresenterMO(qint32 lProductId, QString strDevPath, QObject *parent) :
	ClPresenterUVC(lProductId, strDevPath, parent),
	m_strDevPath( strDevPath )
{
	// 怪しげなXmlに設定するプロダクトID
	m_ProductName	= MO_PRODUCT_NAME;

	m_iZoomInterval			= PARAM_MO1_ZOOM_CMD_SEND_RES;			// ズーム設定値間隔
	m_iFocusInterval		= PARAM_MO1_FOCUS_CMD_SEND_RES;			// フォーカス設定値間隔
	m_iBrightnessInterval	= PARAM_MO1_BRIGHTNESS_CMD_SEND_RES;	// 明度設定値間隔
	m_pclLED	= NULL;
	m_pclRotate = NULL;
#ifndef EU_MO1
	// Extension Unit初期化
	m_stEU1 = stEXTUNIT_TBL[0];
	m_stEU2 = stEXTUNIT_TBL[1];
#endif
}
//--------------------------------------------------------------------------------
/*!
	@fn			~ClPresenterMO
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterMO::~ClPresenterMO()
{
}
//--------------------------------------------------------------------------------
/*!
	@fn			GraphManager構築
 */
//--------------------------------------------------------------------------------
HRESULT ClPresenterMO::buildGraphMgr( void )
{
	if( NULL == m_pclGraphManager )
	{
		return IM_ERROR_NO_MEM;
	}

	// 怪しげなxml
#if 1 //フレームレート指定無(従来)
	QString strXml("<?xml version=\"1.0\" encoding=\"UTF-8\"?><dsvl_input><camera show_format_dialog=\"false\" device_name=\"" + m_strDevPath + "\"><pixel_format><RGB24 flip_h=\"true\" flip_v=\"true\"/></pixel_format></camera></dsvl_input>");
#endif
#if 0 //フレームレート指定有(ELVDLib変更しないと解像度切替するとデフォルトに戻る)
	QString strXml("<?xml version=\"1.0\" encoding=\"UTF-8\"?><dsvl_input><camera frame_rate=\"7.5\" show_format_dialog=\"false\" device_name=\"" + m_strDevPath + "\"><pixel_format><RGB24 flip_h=\"true\" flip_v=\"true\"/></pixel_format></camera></dsvl_input>");
#endif
	qint32	lLength = strXml.length();

	// 配列生成
	char* config_default = new char[lLength];

	// 移植
	for(int i = 0; i < lLength; i++){
		config_default[i] = strXml.data()[i].toLatin1();
	}

	// ConfigによるCameraの初期化
	HRESULT  hr = m_pclGraphManager->buildString(config_default);
	delete[] config_default;

	return hr;
}
/*! ------------------------------------------------------------------------------------- //
//
//		@brief	ウィジェット生成
//
//  ------------------------------------------------------------------------------------- */
void ClPresenterMO::createRemoconWidget()
{
	// ばかよけ
	if(NULL == m_pclGraphManager)
	{
		return;
	}

	// エクステンションユニット準備
	m_pclGraphManager->initKsNode(GUID_877_EXTENSION_UNIT);


	// UVC共通部分の生成
	ClPresenterUVC::createRemoconWidget();

	// リモコンウィジェットはClPresenterUVCのCreateで初期化しているはず
	if( m_pclRemoconWidget == NULL )
	{
		return;
	}

	// 結果代入用
	HRESULT	hr;

	// 各パラメータ代入用
	LONG	lCur;					// 現在値
	LONG	lMin;					// 最小値
	LONG	lMax;					// 最大値
	LONG	lRes;					// 指定可能な精度
	LONG	lDef;					// でふぉ値
	LONG	lFlg;					// フラグ
	ULONG	ulLength;				// 代入変数のサイズ

	// -------------------------------- //
	//
	//		フリッカ
	//
	// -------------------------------- //

	// 代入データのサイズ
	ulLength = sizeof(long);

	// 値取得
	hr = m_pclGraphManager->getParamRange(CC_VideoProcAmp, VideoProcAmp_PowerLineFrequency , &lMin, &lMax, &lRes, &lDef, &lFlg);

	// 操作の余地があるか
	if( lMin != lMax && !FAILED(hr))
	{
		// 生成
		QVBoxLayout*	pclEx1Layout	= new QVBoxLayout;		// レイアウト
		QComboBox*		pclSetter		= new QComboBox;		// ボタン

		// 設定
		QStringList		clItems;
		clItems.clear();

		EXP_MO1_REMOCON_SET_ITEMLIST(clItems);
		pclSetter->addItems(clItems);

		// コネクト
#ifdef __DEBUG__
		qDebug() << "Before connect";
#endif
		connect( pclSetter, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotProcChangeFlicker(QString)));	// 選択項目変更時

		// ウィジェットに追加
		pclEx1Layout->addWidget( pclSetter );

		// 調節内容を表すラベル
		QLabel*	pclName = new QLabel(WORD_MO1_REMOCON_FLICK, m_pclRemoconWidget);
		connect( this, SIGNAL(sigSetTextRemoconFlicker(QString)),
				 pclName, SLOT( setText(QString)) );

		m_pclMainLayout->addWidget( pclName );
		m_pclMainLayout->addLayout( pclEx1Layout );
	}

	// LED切り替えと回転のボタンを横に並べる
	QHBoxLayout*	pclHorizontalLayout = new QHBoxLayout;

	// -------------------------------- //
	//
	//		Ex1 LEDのON・OFFに使用
	//
	// -------------------------------- //

	// 代入データのサイズ
	ulLength = sizeof(long);

	// 値取得
	m_pclGraphManager->getExtensionValues(eKSPROPERTY_877XU_EXTENSION1 ,&ulLength, &lCur, &lMin, &lMax, &lRes, &lDef);

	// 操作の余地があるか
	if( lMin != lMax )
	{
#ifdef __DEBUG__
		qDebug() << "LED" << lMin << ":" << lMax;
#endif
		// 生成
		QVBoxLayout*	pclEx1Layout	= new QVBoxLayout;		// レイアウト
		if( m_pclLED == NULL )
		{
			m_pclLED		= new QPushButton;					// ボタン
		}

		// 設定
		m_pclLED->setCheckable( true );							// ON・OFF状態を作る
		m_pclLED->setText(WORD_MO1_REMOCON_LED);				// テキスト

		// コネクト
#ifdef __DEBUG__
		qDebug() << "Before connect";
#endif
		connect( m_pclLED, SIGNAL(toggled(bool)), this, SLOT(slotProcToggleLED(bool)));	// LEDボタンON・OFF切り替え

		// ウィジェットに追加
		pclEx1Layout->addWidget( m_pclLED );
		pclHorizontalLayout->addLayout( pclEx1Layout );
	}

	// -------------------------------- //
	//
	//		Ex2 画像回転に使用
	//
	// -------------------------------- //

	m_pclGraphManager->getExtensionValues(eKSPROPERTY_877XU_EXTENSION2 ,&ulLength, &lCur, &lMin, &lMax, &lRes, &lDef);
	if( lMin != lMax )
	{
#ifdef __DEBUG__
		qDebug() << "Rotate" << lMin << ":" << lMax;
#endif
		// 生成
		QVBoxLayout*	pclEx2Layout	= new QVBoxLayout;		// レイアウト
		if( m_pclRotate == NULL )
		{
			m_pclRotate		= new QPushButton;					// ボタン
		}

		// 設定
		m_pclRotate->setText(WORD_MO1_REMOCON_ROTATE);			// テキスト

		// コネクト
		connect( m_pclRotate, SIGNAL(clicked()), this, SLOT(slotProcRotate()));	// 押された時の動作

		// ウィジェットに追加
		pclEx2Layout->addWidget( m_pclRotate );
		pclHorizontalLayout->addLayout( pclEx2Layout );
	}

	// Ex1 と Ex2 をメインレイアウトに追加
	m_pclMainLayout->addLayout(pclHorizontalLayout);


	// -------------------------------- //
	//
	//		×ボタン
	//
	// -------------------------------- //
	{

		// ------------
		//　レイアウト
		QHBoxLayout* layoutExit = new QHBoxLayout();
		layoutExit->setSpacing(2);
		layoutExit->setObjectName(QString::fromUtf8("layoutExit"));

		// スペーサ
		QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		layoutExit->addItem(horizontalSpacer);

		// ロールオーバーボタン
		ClRollOverToolBtn* btnExit = new ClRollOverToolBtn(m_pclRemoconWidget);
		btnExit->setObjectName(QString::fromUtf8("btnExit"));
		btnExit->setMinimumSize(QSize(18, 18));
		btnExit->setMaximumSize(QSize(18, 18));
		btnExit->setFocusPolicy(Qt::NoFocus);

		//　アイコン
		QIcon icon;
		icon.addFile(QString::fromUtf8(":/imgs/rsc/Btn_sys_exit_off.png"), QSize(), QIcon::Normal, QIcon::On);
		icon.addFile(QString::fromUtf8(":/imgs/rsc/Btn_sys_exit_down.png"), QSize(), QIcon::Disabled, QIcon::On);
		icon.addFile(QString::fromUtf8(":/imgs/rsc/Btn_sys_exit_on.png"), QSize(), QIcon::Active, QIcon::On);
		icon.addFile(QString::fromUtf8(":/imgs/rsc/Btn_sys_exit_down.png"), QSize(), QIcon::Selected, QIcon::On);
		btnExit->setProperty("imgShow", QVariant(icon));

		layoutExit->addWidget(btnExit);

		QSpacerItem* horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		layoutExit->addItem(horizontalSpacer_2);

		connect( btnExit, SIGNAL(clicked()), m_pclRemoconWidget, SLOT(hide()));

		m_pclMainLayout->addLayout(layoutExit);

	}
}


/*! ------------------------------------------------------------------------------------- //
//
//		@brief	フリッカ補正値セッタ
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterMO::setFlicker(long lParam)
{
	// ローカル変数
	int		iResult = S_OK;	// 結果用
	ULONG	ulLength = 0;	// データ長
	bool	bDummy = false;	// ダミー引数

	ulLength = sizeof(long);

	// ばかよけ
	if(NULL == m_pclGraphManager)
	{
		return IM_ERROR_NOT_OPEN;
	}

	// 値取得
	iResult = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_PowerLineFrequency , lParam,  bDummy);


#ifdef __DEBUG__
	long	lCur;
	iResult = m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_PowerLineFrequency , &lCur,  &bDummy);
	qDebug() << "lCur" << lCur;
#endif
	// 結果判定
	if(iResult != S_OK){

		return IM_ERROR_COMMAND_FAILURE;	// エラー

	}

	return IM_ERROR_SUCCESS;
}


/*! ------------------------------------------------------------------------------------- //
//
//		@brief	フリッカ補正値ゲッタ
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterMO::getFlicker(long &lParam)
{
	// 結果用
	int iResult = S_OK;

	// 各パラメータ代入用
	bool	bDummy = false;				// ダミー引数

	// ばかよけ
	if(NULL == m_pclGraphManager)
	{
		return IM_ERROR_NOT_OPEN;
	}

	// 値取得
	iResult = m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_PowerLineFrequency , &lParam,  &bDummy);

	// 結果判定
	if(iResult != S_OK){

		return IM_ERROR_COMMAND_FAILURE;	// エラー

	}


#ifdef __DEBUG__
	qDebug() << "lParam" << lParam;
#endif
	// 成功
	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	Extension 1 (仕様上LED切り替え
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterMO::setEx1(long lParam)
{
	// ローカル変数
	int		iResult = S_OK;					// 結果用

	// ばかよけ
	if(NULL == m_pclGraphManager)
	{
		return IM_ERROR_NOT_OPEN;
	}
#ifdef EU_MO1								//MO-1/MO-1w Extension Unit
	ULONG	ulLength;						// データ長
	ulLength = sizeof(long);

	// 値設定
	iResult = m_pclGraphManager->setExtensionValue(eKSPROPERTY_877XU_EXTENSION1 ,ulLength, &lParam);

	// 結果判定
	if(iResult != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}
#else										//L-12i UVC Extension Unit (テスト用)
	// Extension Unit情報取得
	getEUInf();

	// 各パラメータ代入用
	ULONG	ulID = m_stEU1.id;									// ID
	ULONG	ulLength = m_stEU1.length;							// 代入変数のサイズ
	unsigned char	*ucSet = new unsigned char [ulLength];		// 設定値

	// 初期化
	for(qint32 i = 0; i < (qint32)ulLength; i++){
		ucSet[i] = 0;
	}

	// 設定する値を格納
	ucSet[0] = lParam;

	// 値設定
	iResult = m_pclGraphManager->setExtensionValue(ulID ,ulLength, ucSet);

	// 結果判定
	if(iResult != S_OK){
		qDebug("set extension values :: id[%d] :: failure!!",ulID);
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}else{
		qDebug("set extension values :: ");
		qDebug(" id[%d] ",ulID);
		qDebug(" length[%d] ",ulLength);
		for(qint32 i = 0; i < (qint32)ulLength; i++){
			qDebug(" cur[0x%02x] ",ucSet[i]);
		}
	}
#endif
	// 成功
	return IM_ERROR_SUCCESS;
}


/*! ------------------------------------------------------------------------------------- //
//
//		@brief	Extension 1 (仕様上LED切り替え
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterMO::getEx1(long &lParam)
{
	// 結果用
	int iResult = S_OK;

	// ばかよけ
	if(NULL == m_pclGraphManager)
	{
		return IM_ERROR_NOT_OPEN;
	}
#ifdef EU_MO1							//MO-1/MO-1w Extension Unit
	// 各パラメータ代入用
	LONG	lCur = 0;					// 現在値
	LONG	lMin = 0;					// 最小値
	LONG	lMax = 0;					// 最大値
	LONG	lRes = 0;					// ？
	LONG	lDef = 0;					// でふぉ値
	ULONG	ulLength = sizeof(LONG);	// 代入変数のサイズ

	// 値取得
	iResult = m_pclGraphManager->getExtensionValues(eKSPROPERTY_877XU_EXTENSION1 ,&ulLength, &lCur, &lMin, &lMax, &lRes, &lDef);

	// 結果判定
	if(iResult != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	//　取得した値をセット
	lParam = lCur;
#ifdef __DEBUG__
	qDebug() << "lParam" << lParam;
	qDebug() << "lCur" << lCur;
#endif
#else									//L-12i UVC Extension Unit (テスト用)
	// Extension Unit情報取得
	getEUInf();

	// 各パラメータ代入用
	ULONG	ulID = m_stEU1.id;									// ID
	ULONG	ulLength = m_stEU1.length;							// 代入変数のサイズ
	unsigned char	*ucCur = new unsigned char [ulLength];		// 現在値
	unsigned char	*ucMin = new unsigned char [ulLength];		// 最小値
	unsigned char	*ucMax = new unsigned char [ulLength];		// 最大値
	unsigned char	*ucRes = new unsigned char [ulLength];		// ステップ
	unsigned char	*ucDef = new unsigned char [ulLength];		// デフォルト

	// 初期化
	for(qint32 i = 0; i < (qint32)ulLength; i++){
		ucCur[i] = 0;
	}

	// 値取得
	iResult = m_pclGraphManager->getExtensionValues(ulID ,&ulLength, ucCur, ucMin, ucMax, ucRes, ucDef);

	// 結果判定
	if(iResult != S_OK){
		qDebug("get extension values :: id[%d] :: failure!!",ulID);
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}else{
		qDebug("get extension values :: ");
		qDebug(" id[%d] ",ulID);
		qDebug(" length[%d] ",ulLength);
		for(qint32 i = 0; i < (qint32)ulLength; i++){
			qDebug(" cur[0x%02x] \t min[0x%02x] \t max[0x%02x] \t res[0x%02x] \t def[0x%02x] ",ucCur[i], ucMin[i], ucMax[i], ucRes[i], ucDef[i]);
		}
	}

	lParam = (long)ucCur[0];
#endif
	// 成功
	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	Extension 2 (仕様上映像回転
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterMO::setEx2(long lParam)
{
	// ローカル変数
	int		iResult = S_OK;					// 結果用

	// ばかよけ
	if(NULL == m_pclGraphManager)
	{
		return IM_ERROR_NOT_OPEN;
	}
#ifdef EU_MO1								//MO-1/MO-1w Extension Unit
	ULONG	ulLength;						// データ長

	// データ長
	ulLength = sizeof(long);

	// 値設定
	iResult = m_pclGraphManager->setExtensionValue(eKSPROPERTY_877XU_EXTENSION2 ,ulLength, &lParam);

	// 結果判定
	if(iResult != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}
#else										//L-12i UVC Extension Unit (テスト用)
	// Extension Unit情報取得
	getEUInf();

	// 各パラメータ代入用
	ULONG	ulID = m_stEU2.id;									// ID
	ULONG	ulLength = m_stEU2.length;							// 代入変数のサイズ
	unsigned char	*ucSet = new unsigned char [ulLength];		// 設定値

	// 初期化
	for(qint32 i = 0; i < (qint32)ulLength; i++){
		ucSet[i] = 0;
	}

	// 設定する値を格納
	ucSet[0] = lParam;

	// 値設定
	iResult = m_pclGraphManager->setExtensionValue(ulID ,ulLength, ucSet);

	// 結果判定
	if(iResult != S_OK){
		qDebug("set extension values :: id[%d] :: failure!!",ulID);
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}else{
		qDebug("set extension values :: ");
		qDebug(" id[%d] ",ulID);
		qDebug(" length[%d] ",ulLength);
		for(qint32 i = 0; i < (qint32)ulLength; i++){
			qDebug(" cur[0x%02x] ",ucSet[i]);
		}
	}
#endif
	// 成功
	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	Extension 2 (仕様上映像回転
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterMO::getEx2(long &lParam)
{
	// 結果用
	int iResult = S_OK;

	// ばかよけ
	if(NULL == m_pclGraphManager)
	{
		return IM_ERROR_NOT_OPEN;
	}
#ifdef EU_MO1							//MO-1/MO-1w Extension Unit
	// 各パラメータ代入用
	LONG	lCur = 0;					// 現在値
	LONG	lMin = 0;					// 最小値
	LONG	lMax = 0;					// 最大値
	LONG	lRes = 0;					// ？
	LONG	lDef = 0;					// でふぉ値
	ULONG	ulLength = sizeof(LONG);	// 代入変数のサイズ

	// 値取得
	iResult = m_pclGraphManager->getExtensionValues(eKSPROPERTY_877XU_EXTENSION2 ,&ulLength, &lCur, &lMin, &lMax, &lRes, &lDef);

	// 結果判定
	if(iResult != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	//　取得した値をセット
	lParam = lCur;
#ifdef __DEBUG__
	qDebug() << "lParam" << lParam;
	qDebug() << "lCur" << lCur;
#endif
#else									//L-12i UVC Extension Unit (テスト用)
	// Extension Unit情報取得
	getEUInf();

	// 各パラメータ代入用
	ULONG	ulID = m_stEU2.id;									// ID
	ULONG	ulLength = m_stEU2.length;							// 代入変数のサイズ
	LONG	*ucCur = new LONG [ulLength];		// 現在値
	unsigned char	*ucMin = new unsigned char [ulLength];		// 最小値
	unsigned char	*ucMax = new unsigned char [ulLength];		// 最大値
	unsigned char	*ucRes = new unsigned char [ulLength];		// ステップ
	unsigned char	*ucDef = new unsigned char [ulLength];		// デフォルト

	// 初期化
	for(qint32 i = 0; i < (qint32)ulLength; i++){
		ucCur[i] = 0;
	}

	// 値取得
	iResult = m_pclGraphManager->getExtensionValues(ulID ,&ulLength, ucCur, ucMin, ucMax, ucRes, ucDef);

	// 結果判定
	if(iResult != S_OK){
		qDebug("get extension values :: id[%d] :: failure!!",ulID);
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}else{
		qDebug("get extension values :: ");
		qDebug(" id[%d] ",ulID);
		qDebug(" length[%d] ",ulLength);
		for(qint32 i = 0; i < (qint32)ulLength; i++){
			qDebug(" cur[0x%02x] \t min[0x%02x] \t max[0x%02x] \t res[0x%02x] \t def[0x%02x] ",ucCur[i], ucMin[i], ucMax[i], ucRes[i], ucDef[i]);
		}
	}

	lParam = (long)ucCur[0];
#endif
	// 成功
	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	言語変換
//
//  ------------------------------------------------------------------------------------- */
void ClPresenterMO::setTranslator()
{
	ClPresenterUVC::setTranslator();
	emit sigSetTextRemoconFlicker(WORD_MO1_REMOCON_FLICK);
	m_pclLED->setText(WORD_MO1_REMOCON_LED);
	m_pclRotate->setText(WORD_MO1_REMOCON_ROTATE);
}


/*! ------------------------------------------------------------------------------------- //
//
//		@brief	フリッカ補正変更
//
//  ------------------------------------------------------------------------------------- */
void ClPresenterMO::slotProcChangeFlicker(QString strItem)
{
	if( strItem == WORD_MO1_REMOCON_FLICK_DISABLE )			// 無効
	{
		setFlicker( eMO1_FLICKER_OFF );
	}
	else if( strItem == WORD_MO1_REMOCON_FLICK_50HZ )		// 50Hz
	{
		setFlicker( eMO1_FLICKER_50 );
	}
	else if( strItem == WORD_MO1_REMOCON_FLICK_60HZ )		// 60Hz
	{
		setFlicker( eMO1_FLICKER_60 );
	}
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	LEDのON・OFF
//
//  ------------------------------------------------------------------------------------- */
void ClPresenterMO::slotProcToggleLED(bool bState)
{
	// ローカル変数
	long	lCur;	// 現在の値取得用

	// 現在値取得
	this->getEx1(lCur);

#ifdef __DEBUG__
	qDebug() << "Before LED" << lCur;
#endif

	// トグル
	if( bState == MO_BUTTON_STATE_TO_LED_STATE_ON )			// ** ボタンON　で　LED　ON ** //
	{
#ifdef __DEBUG__
		qDebug() << "Set LED" << eMO1_LED_ON;
#endif
		this->setEx1(eMO1_LED_ON);
	}
	else if( bState == MO_BUTTON_STATE_TO_LED_STATE_OFF )	// ** ボタンOFF　で　LED　OFF ** //
	{
#ifdef __DEBUG__
		qDebug() << "Set LED" << eMO1_LED_OFF;
#endif
		this->setEx1(eMO1_LED_OFF);
	}

#ifdef __DEBUG__
//	this->getEx1(lCur);
//	qDebug() << "After LED" << lCur;
#endif
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	映像反転
//
//  ------------------------------------------------------------------------------------- */
void ClPresenterMO::slotProcRotate()
{
	// ローカル変数
	long	lCur;	// 現在の値取得用

	// 現在値取得
	this->getEx2(lCur);

//#ifdef __DEBUG__
//	qDebug() << "Before Rotate" << lCur;
//#endif

	// トグル
	if( lCur == eMO1_ROTATE_ON )
	{
		lCur = eMO1_ROTATE_OFF;
	}
	else
	{
		lCur = eMO1_ROTATE_ON;
	}


//#ifdef __DEBUG__
//	qDebug() << "Set Rotate" << lCur;
//#endif

	// セット
	this->setEx2( lCur );

#ifdef __DEBUG__
	// 現在値取得
	this->getEx2(lCur);

	qDebug() << "After Rotate" << lCur;
#endif
}
#ifndef EU_MO1
//--------------------------------------------------------------------------------
/*!
	@fn			getModelName
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClPresenterMO::getEUInf()
{
	// テキストファイルからどのExtension Unitを使うかを取得
	QFile file("EXTUNIT_HICS.txt");
	if(file.open(QFile::ReadOnly))
	{
		// 1行目 (Extension Unit 1)
		QString clEU1(file.readLine());
		if( true == clEU1.contains(QString("Extension Unit 1 = ")) )
		{
			qint32 index = clEU1.remove(QString("Extension Unit 1 = ")).toInt() - 1;	//配列のindexにあわせるために1引く
			m_stEU1 = stEXTUNIT_TBL[index];
		}

		// 2行目 (Extension Unit 2)
		QString clEU2(file.readLine());
		if( true == clEU2.contains(QString("Extension Unit 2 = ")) )
		{
			qint32 index = clEU2.remove(QString("Extension Unit 2 = ")).toInt() - 1;	//配列のindexにあわせるために1引く
			m_stEU2 = stEXTUNIT_TBL[index];
		}

		file.close();
	}
}
#endif


//--------------------------------------------------------------------------------
/*!
	@fn			getModelName
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO::getModelName(QVariant &qParam)
{
	qParam = MO_PRODUCT_NAME;						//output model name to QVariant
	return IM_ERROR_SUCCESS;
}
