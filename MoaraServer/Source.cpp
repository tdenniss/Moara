#include <QCoreApplication>

#include "MainServer.h"

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	MainServer server;
	const quint16 port = 12345;

	if (!server.Start(port))
		return -1;

	return a.exec();
}
