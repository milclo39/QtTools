#include <QApplication>
#include "ClQmlApplicationEngine.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ClQmlApplicationEngine obj;

    return app.exec();
}
