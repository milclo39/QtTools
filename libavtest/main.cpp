//#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#ifdef Q_OS_WIN
#include <Windows.h>
#include <tlhelp32.h>
#include <QString>
#endif
#include "ClStreamView.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    //QGuiApplication app(argc, argv);
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    qmlRegisterType<ClStreamView>("elmo.ClStreamView", 1, 0, "ClStreamView");

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
