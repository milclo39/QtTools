#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QTextCodec>
#include <QQuickView>

#include "ClStreamView.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());

	qmlRegisterType<ClStreamView>("elmo.ClStreamView", 1, 0, "ClStreamView");
	QQmlApplicationEngine engine("main.qml");

	return app.exec();
}
