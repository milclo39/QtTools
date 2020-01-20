#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTextCodec>
#include "Drawing.h"
#include <QWidget>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());

	qmlRegisterType<Drawing>("DrawingArea", 1, 0, "Drawing");
	qmlRegisterType<ClDrawParts>("ObjectArea", 1, 0, "Objecting");
	qmlRegisterType<ClDrawFreeLine>("FreeLineArea", 1, 0, "FreeLining");
	qmlRegisterType<ClDrawImage>("ImageArea", 1, 0, "Imageing");
	QQmlApplicationEngine engine("qml/main.qml");
#if 0
	QObject *object = engine.rootObjects().first();
	QWidget *widget = new QWidget();
	QObject::connect(object, SIGNAL(qmlSignal()), widget, SLOT(close()));
#endif
	return app.exec();
}
