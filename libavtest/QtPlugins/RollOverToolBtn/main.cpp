//***************************************************************************//
//***************************************************************************//
/*!
	@file		main.cpp

	@brief		メイン

	@author		株式会社エルモ社  映像技術第二部

	Confidential

	Copyright(c) Elmo Industrial Co.,Ltd.		http://www.elmo.co.jp/
*/
//***************************************************************************//
//***************************************************************************//
#include	<QtGui/QApplication>
#include	"RollOverToolBtn/ClRollOverToolBtn.h"


int
main( int argc, char *argv[] )
{
	QApplication  clApp( argc, argv );

	QWidget				clWidget;
	ClRollOverToolBtn	clBtn(&clWidget);

	QIcon  clIcon;
	clIcon.addFile( QString( "C:\\Documents and Settings\\takai_t\\My Documents\\My Pictures\\00.png" ),
					QSize( 0, 0 ),
					QIcon::Normal,
					QIcon::Off );
	clIcon.addFile( QString( "C:\\Documents and Settings\\takai_t\\My Documents\\My Pictures\\00.png" ),
					QSize( 0, 0 ),
					QIcon::Normal,
					QIcon::On );
	clIcon.addFile( QString( "C:\\Documents and Settings\\takai_t\\My Documents\\My Pictures\\01.png" ),
					QSize( 0, 0 ),
					QIcon::Active,
					QIcon::Off );
	clIcon.addFile( QString( "C:\\Documents and Settings\\takai_t\\My Documents\\My Pictures\\01.png" ),
					QSize( 0, 0 ),
					QIcon::Active,
					QIcon::On );
	clIcon.addFile( QString( "C:\\Documents and Settings\\takai_t\\My Documents\\My Pictures\\02.png" ),
					QSize( 0, 0 ),
					QIcon::Selected,
					QIcon::Off );
	clIcon.addFile( QString( "C:\\Documents and Settings\\takai_t\\My Documents\\My Pictures\\02.png" ),
					QSize( 0, 0 ),
					QIcon::Selected,
					QIcon::On );
	clIcon.addFile( QString( "C:\\Documents and Settings\\takai_t\\My Documents\\My Pictures\\03.png" ),
					QSize( 0, 0 ),
					QIcon::Disabled,
					QIcon::Off );
	clIcon.addFile( QString( "C:\\Documents and Settings\\takai_t\\My Documents\\My Pictures\\03.png" ),
					QSize( 0, 0 ),
					QIcon::Disabled,
					QIcon::On );
	clBtn.setImgShow( clIcon );

QRect  clRect = clBtn.geometry();
clRect.setWidth( 100 );
clRect.setHeight( 100 );
clBtn.setGeometry( clRect );

	clWidget.updateGeometry();
	clWidget.show();
	int  iRet = clApp.exec();

	return iRet;
}
