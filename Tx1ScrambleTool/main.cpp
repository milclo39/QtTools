#include <QApplication>
#include <QResource>
#include <QMessageBox>

#include "widget.h"
#include "AppCheck/ClAppCheck.h"

#define APPLICATION_NAME	QObject::tr("TX-1_ScrambleTool")
#define APPLICATION_VER		( "00.00.001" )
#define APPLICATION_MULTI_MUTEX_NAME	("Scramble")
#define	APP_TXT_ERR_ALREADY_RUNNING		( ClAppCheck::tr( "%1 is already running" ))	// 既に起動していた
#define	APP_TXT_ERR_OTHER_USER			( ClAppCheck::tr( "Unable to start because another user is running %1" ))	// 他ユーザが起動中
#define	APP_TXT_ERR_INIT				( ClAppCheck::tr( "Unable to initialize %1" ))	// その他エラーが発生した

//--------------------------------------------------------------------------------//
/*!
	@brief		メイン
	@param[in]	argc : 引数数
	@param[in]	argv : 引数
*/
//--------------------------------------------------------------------------------//
int main(int argc, char *argv[])
{
	QApplication clApp(argc, argv);

	//initTranslator(":/lang/%1");
	clApp.setApplicationName(APPLICATION_NAME);
	clApp.setApplicationVersion(APPLICATION_VER);

	//多重起動確認
	ClAppCheck clCheck(clApp.applicationName());	//多重起動確認
	ClAppCheck::emEXIST eRet = ClAppCheck::DOIEXIST_ERROR;

	eRet = clCheck.doIExist(APPLICATION_MULTI_MUTEX_NAME);
	if(eRet != ClAppCheck::DOIEXIST_NOTEXIST)
	{
		QString strError;
		switch( eRet )
		{
			//	既に起動している
			case ClAppCheck::DOIEXIST_EXIST_SELF:
			{
				strError = APP_TXT_ERR_ALREADY_RUNNING;
				break;
			}
			//	他のユーザが実行している
			case ClAppCheck::DOIEXIST_EXIST_OTHER:
			{
				strError = APP_TXT_ERR_OTHER_USER;
				break;
			}
			//	エラー
			default:
			{
				strError = APP_TXT_ERR_INIT;
				break;
			}
		}
		strError = strError.arg( qApp->applicationName() );
		QMessageBox clMsgBox( QMessageBox::NoIcon, qApp->applicationName(), strError, QMessageBox::Ok );
		clMsgBox.exec();
		return -1;
	}

	Widget w;
	w.setWindowTitle( APPLICATION_NAME + "   " + "Ver." + APPLICATION_VER );
	w.initalize();
	w.show();

	return clApp.exec();
}
