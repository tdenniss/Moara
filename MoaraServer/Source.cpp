#include "SfmlServer.h"

#include <iostream>

int PORT = 53000;

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		PORT = std::stoi(argv[1]);
	}

	SfmlServer server;

	if (!server.Init(PORT))
	{
		std::cerr << "Error: Could not listen on port " << PORT << std::endl;
		return -1;
	}

	server.Start();

	return 0;
}