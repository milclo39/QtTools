// --------------------------------------------------------------------------------------------- //
  /*! =========================================================================================

		@file   ClMainWindow.h

		@author （Ｓ技１）熊崎

		@brief  テストメインウィンドウ

	==========================================================================================　*/
// --------------------------------------------------------------------------------------------- //

#ifndef CLMAINWINDOW_H
#define CLMAINWINDOW_H

#include <QMainWindow>


namespace Ui {
    class ClMainWindow;
}

class ClAdaptRicohDeviceCtl;

#define	_CLMAINWINDOW_LOG_TAB_WIDTH	(8)	//!< ログのタブの幅

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@ingroup    grpRicohLibTest

			@brief		テストメインウィンドウ

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
class ClMainWindow : public QMainWindow
{
    Q_OBJECT

public:
	explicit ClMainWindow(QWidget *parent = 0);
    ~ClMainWindow();

signals:

	// ログ
	void sigAppendLog( QString strLog );

	// セッター
	void sigSetPan( qlonglong qlParam );
	void sigSetZoom( qlonglong qlParam );

	void sigSetBrightness( qlonglong qlParam );

private slots:

	// ---------------------------------- //
	//	Camera Terminal Controls
	// ---------------------------------- //

	// パン
	void slotGetPan();
	void slotSetPan();

	// ズーム
	void slotGetZoom();
	void slotSetZoom();

	// ---------------------------------- //
	//	Video Prosessing Unit Controls
	// ---------------------------------- //

	// 明度
	void slotGetBrightness();
	void slotSetBrightness();

private:

	// UI
	Ui::ClMainWindow *m_pclUI;

	// リコーカメラコントローラ
	ClAdaptRicohDeviceCtl* m_pclDeviceCtl;
};

#endif // CLMAINWINDOW_H
