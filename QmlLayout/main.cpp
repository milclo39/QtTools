#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "ElWorker.h"

int main(int argc, char* argv[])
{
	QGuiApplication app(argc, argv);

	ElWorker::registerElmoLibrary();

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

//	QObject *root = engine.rootObjects().first();
//	QObject::connect(root, SIGNAL(sigSelectQuality()), root, SLOT(slotSelectQuality()));

	return app.exec();
}
