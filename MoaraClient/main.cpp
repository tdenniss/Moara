#include "MoaraClient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MoaraClient w;
    w.showFullScreen();
    return a.exec();
}
