#include "MoaraClient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MoaraClient w;
    w.show();
    return a.exec();
}
