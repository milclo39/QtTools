// --------------------------------------------------------------------------------------------- //
  /*! =========================================================================================

		@file   ClMainWindow.h

		@author （Ｓ技１）熊崎

		@brief  テストメインウィンドウ

	==========================================================================================　*/
// --------------------------------------------------------------------------------------------- //

#include "ClMainWindow.h"
#include "ui_ClMainWindow.h"

#include "ClAdaptRicohDeviceCtl.h"

#include <QDebug>

typedef ClAdaptRicohDeviceCtl::stUVC_PARAM stUVC_PARAM;

// 表示内容
#define _CLMAINWINDOW_SUCCESS_PARAM_EXP ("\tCur\t: %1 \n\tMin\t: %2 \n\tMax\t: %3 \n\tRes\t: %4 \n\tDef\t: %5")

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		コンストラクタ

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
ClMainWindow::ClMainWindow(QWidget *parent) :
    QMainWindow(parent),
	m_pclUI(new Ui::ClMainWindow)
{

	m_pclUI->setupUi(this);

	m_pclDeviceCtl =  new ClAdaptRicohDeviceCtl;

	m_pclUI->clTextBrowserLog->setTabStopWidth(_CLMAINWINDOW_LOG_TAB_WIDTH);

	// ログ出力
	connect( this, SIGNAL(sigAppendLog(QString)),
			 m_pclUI->clTextBrowserLog, SLOT(append(QString)));

	// ---------------------------------- //
	//	Camera Terminal Controls
	// ---------------------------------- //

	// パン
	connect( m_pclUI->clPushButtonGetPan, SIGNAL( clicked() ),
			 this, SLOT( slotGetPan() ));
	connect( m_pclUI->clPushButtonSetPan, SIGNAL( clicked() ),
			 this, SLOT( slotSetPan() ));
	connect( this, SIGNAL( sigSetPan(qlonglong) ),
			 m_pclDeviceCtl, SLOT(slotSetPan(qlonglong)));

	// ズーム
	connect( m_pclUI->clPushButtonGetZoom, SIGNAL( clicked() ),
			 this, SLOT( slotGetZoom() ));
	connect( m_pclUI->clPushButtonSetZoom, SIGNAL( clicked() ),
			 this, SLOT( slotSetZoom() ));
	connect( this, SIGNAL( sigSetZoom(qlonglong) ),
			 m_pclDeviceCtl, SLOT(slotSetZoom(qlonglong)));

	// ---------------------------------- //
	//	Video Prosessing Unit Controls
	// ---------------------------------- //

	// 明度
	connect( m_pclUI->clPushButtonGetBrightness, SIGNAL( clicked() ),
			 this, SLOT( slotGetBrightness() ));
	connect( m_pclUI->clPushButtonSetBrightness, SIGNAL( clicked() ),
			 this, SLOT( slotSetBrightness() ));
	connect( this, SIGNAL( sigSetBrightness(qlonglong) ),
			 m_pclDeviceCtl, SLOT(slotSetBrightness(qlonglong)));

}

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		デストラクタ

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
ClMainWindow::~ClMainWindow()
{
	// ログ出力
	connect( this, SIGNAL(sigAppendLog(QString)),
			 m_pclUI->clTextBrowserLog, SLOT(append(QString)));

	// ---------------------------------- //
	//	Camera Terminal Controls
	// ---------------------------------- //

	// パン
	disconnect( m_pclUI->clPushButtonGetPan, SIGNAL( clicked() ),
			 this, SLOT( slotGetPan() ));
	disconnect( m_pclUI->clPushButtonSetPan, SIGNAL( clicked() ),
			 this, SLOT( slotSetPan() ));
	disconnect( this, SIGNAL( sigSetPan(qlonglong) ),
			 m_pclDeviceCtl, SLOT(slotSetPan(qlonglong)));

	// ズーム
	disconnect( m_pclUI->clPushButtonGetZoom, SIGNAL( clicked() ),
			 this, SLOT( slotGetZoom() ));
	disconnect( m_pclUI->clPushButtonSetZoom, SIGNAL( clicked() ),
			 this, SLOT( slotSetZoom() ));
	disconnect( this, SIGNAL( sigSetZoom(qlonglong) ),
			 m_pclDeviceCtl, SLOT(slotSetZoom(qlonglong)));

	// ---------------------------------- //
	//	Video Prosessing Unit Controls
	// ---------------------------------- //

	// 明度
	disconnect( m_pclUI->clPushButtonGetBrightness, SIGNAL( clicked() ),
			 this, SLOT( slotGetBrightness() ));
	disconnect( m_pclUI->clPushButtonSetBrightness, SIGNAL( clicked() ),
			 this, SLOT( slotSetBrightness() ));
	disconnect( this, SIGNAL( sigSetBrightness(qlonglong) ),
			 m_pclDeviceCtl, SLOT(slotSetBrightness(qlonglong)));

	delete m_pclUI;
}

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		パン　ゲット

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
void ClMainWindow::slotGetPan()
{/*
	stUVC_PARAM stVal;
	if( m_pclDeviceCtl->getBrightness( &stVal ) == false )	// ** if 失敗 ** //
	{
		emit sigAppendLog("Get pan value failed.");
	}
	else{											// ** if 成功 ** //
		emit sigAppendLog( QString( "Get pan value successed. " ));
		emit sigAppendLog( QString( _CLMAINWINDOW_SUCCESS_PARAM_EXP )
						   .arg( stVal.qlCur )
						   .arg( stVal.qlMin )
						   .arg( stVal.qlMax )
						   .arg( stVal.qlRes )
						   .arg( stVal.qlDef ) );
	}*/
}

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		パン　セット

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
void ClMainWindow::slotSetPan()
{
/*	QString strValue = m_pclUI->clLineEditPan->text();
	if( m_pclDeviceCtl->slotSetPan( strValue.toLongLong()) == false )	// ** 失敗 ** //
	{
		emit sigAppendLog("Set pan value failed.");
	}
	else																// ** if 成功 ** //
	{
		emit sigAppendLog("Set pan value successed.");
	}*/
}


// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		ズーム　ゲット

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
void ClMainWindow::slotGetZoom()
{
	stUVC_PARAM stVal;
	if( m_pclDeviceCtl->getZoom( &stVal ) == false )	// ** if 失敗 ** //
	{
		emit sigAppendLog("Get zoom value failed.");
	}
	else{												// ** if 成功 ** //
		emit sigAppendLog( QString( "Get zoom value successed. " ));
		emit sigAppendLog( QString( _CLMAINWINDOW_SUCCESS_PARAM_EXP )
						   .arg( stVal.qlCur )
						   .arg( stVal.qlMin )
						   .arg( stVal.qlMax )
						   .arg( stVal.qlRes )
						   .arg( stVal.qlDef ) );
	}
}

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		ズーム　セット

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
void ClMainWindow::slotSetZoom()
{
	QString strValue = m_pclUI->clLineEditZoom->text();
	if( m_pclDeviceCtl->slotSetZoom( strValue.toLongLong()) == false )	// ** 失敗 ** //
	{
		emit sigAppendLog("Set zoom value failed.");
	}
	else																// ** if 成功 ** //
	{
		emit sigAppendLog("Set zoom value successed.");
	}
}


// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		明度　ゲット

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
void ClMainWindow::slotGetBrightness()
{
	stUVC_PARAM stVal;

	if( m_pclDeviceCtl->getBrightness( &stVal ) == false )	// ** if 失敗 ** //
	{
		emit sigAppendLog("Get brightness value failed.");
	}
	else													// ** if 成功 ** //
	{
		emit sigAppendLog( QString( "Get brightness value successed. " ));
		emit sigAppendLog( QString( _CLMAINWINDOW_SUCCESS_PARAM_EXP )
						   .arg( stVal.qlCur )
						   .arg( stVal.qlMin )
						   .arg( stVal.qlMax )
						   .arg( stVal.qlRes )
						   .arg( stVal.qlDef ) );
	}
}

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@brief		明度　セット

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
void ClMainWindow::slotSetBrightness()
{
	QString strValue = m_pclUI->clLineEditBrightness->text();
	if( m_pclDeviceCtl->slotSetBrightness( strValue.toLongLong()) == false )	// ** if 失敗 ** //
	{
		emit sigAppendLog("Set brightness value failed.");
	}
	else																		// ** if 成功 ** //
	{
		emit sigAppendLog("Set brightness value successed.");
	}
}
