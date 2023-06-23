#include <QtGui/QApplication>
#include "ClMainWindow.h"

#pragma	comment(linker,"/NODEFAULTLIB:\"LIBCMTD.lib\"")


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClMainWindow w;
    w.show();

    return a.exec();
}
