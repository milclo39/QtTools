#include <QtGui/QApplication>
#include "mainwindow.h"
#include "Common/ElmoStruct.h"

//#include "qtsingleapplication.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

//	if(a.isRunning() == true){
//		exit(0);
//	}

#if 1	
	//ツールでキャプチャ画像を保存できなかったので追加(IMA4SRS MO-1 Mac対応)
#if defined(Q_OS_WIN)
	a.addLibraryPath(qApp->applicationDirPath() + "/imageformats");
	a.addLibraryPath(qApp->applicationDirPath() + "/sqldrivers");
	a.addLibraryPath(qApp->applicationDirPath() + "/phonon_backend");
#elif defined(Q_OS_MAC)
	a.addLibraryPath("/opt/ELMO/IMA4SRS");
	a.addLibraryPath("/opt/ELMO/IMA4SRS/plugins");
	a.addLibraryPath("/opt/ELMO/IMA4SRS/plugins/imageformats");
	a.addLibraryPath("/opt/ELMO/IMA4SRS/plugins/phonon_backend");
#endif
#endif

    MainWindow w;

    w.show();
	return a.exec();
}
