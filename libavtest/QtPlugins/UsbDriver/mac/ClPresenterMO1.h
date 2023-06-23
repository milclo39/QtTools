#ifndef CLPRESENTERMO1_MAC_H
#define CLPRESENTERMO1_MAC_H

#if 0															//デバッグ用
#define DISPLAY_TRACE
#include <QDebug>
#endif

#include "mac/ClDeviceInfo_mac.h"
#include "ClUsbDriver/ClUsbDriver_global.h"
#if 0
#include "mac/ClPresenterMO1Wrap.hpp"
#else
#include "mac/ClPresenterMO1Wrap.h"
#endif

#include <stdbool.h>

#include <QByteArray>
#include <QStringList>

#include "ClPresenterMO1.h"


//--------------------------------------------------------------------------------//
/*!
	@brief		MO-1/MO-1w制御クラス
*/
//--------------------------------------------------------------------------------------
class ClPresenterMO1 : public ClDeviceInfo
{
	Q_OBJECT

public:
	ClPresenterMO1(QObject *pqParent = 0);
	virtual ~ClPresenterMO1();

	//オーバーライド
public:
	virtual qint32		open();									//open camera
	virtual void		close();								//close camera
	virtual qint32		CommandPresenter( qint32 CmdID,			//
										  QVariant& qParameter );
	virtual void		setConnected(bool bIsConnect);			//connect

	// ----------------------------------------------------------- //
	//								UVC
	// ------------------------------------------------------------//
	//リモコン
	virtual QWidget*	getRemoconWidget();
	//解像度
	QStringList			getSupportedResolutions();
	QString				getCurrentResolution();
	qint32				setCurrentResolution(QString strResolution);

protected:
	virtual qint32		getPicture( QVariant &qParam );			//get picture from presenter
	virtual qint32		getModelName( QVariant &qParam );		//get model name from device and output to QVariant

	virtual qint32		execCommand( ClUsbCmd &clCmd )			//execute some command with device
	{
		Q_UNUSED(clCmd)											//未使用定義
		return 0;												//ひとまず無いので0を返すようにしておく
	}
	virtual qint32		getVersion( QVariant &qParam )			//get version from device and output to QVariant
	{
		Q_UNUSED(qParam)										//未使用定義
		return 0;												//ひとまず無いので0を返すようにしておく
	}
	virtual qint32		execCaptureInit()						//CaptureFlag
	{
		return 0;												//ひとまず無いので0を返すようにしておく
	}
	virtual qint32		getCaptureStatus( QVariant &Param )		//function of get CaptureFlag
	{
		Q_UNUSED(Param)											//未使用定義
		return 0;												//ひとまず無いので0を返すようにしておく
	}
	
	void				startCmdTimer( qint32 lCmd );			//start timer
	void				stopCmdTimer();							//stop timer

	//スロット
private slots:
	void				slotTimeout();							//timer time out

	//メンバー関数
public:
	void				MOCameraLibCall( bool Add_Remove );		//call back
	bool				isOpen(){return m_bIsConnect;}			//check device state (open/close)

private:
	void				init();

	qint32				focusAuto( QVariant &qParam );			//focus auto
	qint32				focusNear( QVariant &qParam );			//focus near
	qint32				focusFar( QVariant &qParam );			//focus far
	qint32				focusStop( QVariant &qParam );			//focus stop

	qint32				zoomTele( QVariant &qParam );			//zoom tele
	qint32				zoomWide( QVariant &qParam );			//zoom wide
	qint32				zoomStop( QVariant &qParam );			//zoom stop

	qint32				irisOpen( QVariant &qParam );			//iris open
	qint32				irisClose( QVariant &qParam );			//iris close
	qint32				irisStop( QVariant &qParam );			//iris stop
    qint32				irisInitialize( QVariant &qParam );		//iris initialize　2013.01.23 L-12i対応のため追加

	bool				getAutoFucus( void );					//get auto focus state (false:off,true:on)
	void				setAutoFucus( bool bValue );			//set auto focus state (false:off,true:on)

	void				getFocusInf( int &iValue,				//get focus information
									 int &iMin, int &iMax );
	void				setFocus( int iValue );					//set focus

	void				getBrightnessInf( int &iValue,			//get brightness information
										  int &iMin, int &iMax );
	void				setBrightness( int iValue );			//set brightness

	void				getZoomInf( int &iValue,				//get zoom information
									int &iMin, int &iMax );
	void				setZoom( int iValue );					//set zoom

	void				startSession();							//start capture session
	void				stopSession();							//stop capture session

	bool				openAudioDevice();						//open audio device

	//メンバー変数
protected:
	QTimer				m_clTimer;								//timer
	qint32				m_lCurrentCmd;							//current exec command
	
	qint32				m_lFocusInterval;						//focus step
	qint32				m_lZoomInterval;						//zoom step
	qint32				m_lBrightnessInterval;					//brightness step

	qint32				m_lFocusDefault;						//focus default
	qint32				m_lZoomDefault;							//zoom default
	qint32				m_lBrightnessDefault;					//brightness default

	qint32				m_lFocusCurrent;						//current focus
	qint32				m_lZoomCurrent;							//current zoom
	qint32				m_lBrightnessCurrent;					//current brightness

private:
	bool				m_bIsConnect;							//connect flag

	//MO-1制御用メンバー変数
private:
	char*				m_pcImageData;							//image data
	int					m_iCurrentResolution;					//current resolution
};
//--------------------------------------------------------------------------------------


#endif // CLPRESENTERMO1_MAC_H
