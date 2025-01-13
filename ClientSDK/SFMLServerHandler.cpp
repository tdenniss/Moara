#include "SFMLServerHandler.h"

#include <iostream>

SFMLServerHandler::SFMLServerHandler()
	: m_serverAddress()
	, m_port(0)
	, m_running(false)
	, m_thread(&SFMLServerHandler::Listen, this)
{
}

SFMLServerHandler::~SFMLServerHandler()
{
	Stop();
}

bool SFMLServerHandler::Start(const std::string& host, int port)
{
	m_serverAddress = host;
	m_port = port;

	if (!ConnectToServer())
		return false;

	m_running = true;
	m_thread.launch();

	return true;
}

void SFMLServerHandler::Stop()
{
	if (m_running)
	{
		m_running = false;

		m_thread.wait();
	}

	DisconnectFromServer();
}

void SFMLServerHandler::SendRequest(const void* message, int size)
{
	if (m_socket->send(message, size + 1) != sf::Socket::Done)
	{
		//qDebug() << "Error: Could not send data";
		return;
	}
}

void SFMLServerHandler::SetNewDataCallback(NewDataCallback newDataFunction)
{
	m_newDataFunction = newDataFunction;
}

void SFMLServerHandler::SetDisconnectedCallback(DisconnectedCallback newDisconnectedFunction)
{
	m_disconnectedFunction = newDisconnectedFunction;
}

void SFMLServerHandler::Listen()
{
	sf::SocketSelector selector;
	selector.add(*m_socket);

	while (m_running)
	{
		if (selector.wait(sf::milliseconds(50)))
		{
			if (selector.isReady(*m_socket))
			{
				std::vector<char> buffer(1024);
				std::size_t received = 0;

				sf::Socket::Status status = m_socket->receive(buffer.data(), buffer.size(), received);

				switch (status)
				{
				case sf::Socket::Status::Done:
				{
					//qDebug() << "Received " << received << " bytes: " << buffer.data();

					try
					{
						if (m_newDataFunction)
							m_newDataFunction(buffer.data());
					}
					catch (const std::exception& e)
					{
						std::cerr << "Error parsing JSON: " << e.what() << std::endl;
					}

					break;
				}
				case sf::Socket::Status::Disconnected: //!< The TCP socket has been disconnected
					m_running = false;
					std::cerr << "disconnected" << std::endl;
					if (m_disconnectedFunction)
						m_disconnectedFunction();
					break;
				case sf::Socket::Status::NotReady:     //!< The socket is not ready to send / receive data yet
				case sf::Socket::Status::Partial:      //!< The socket sent a part of the data
				case sf::Socket::Status::Error:
					m_running = false;
					std::cerr << "Error receiving data (" << status << ")" << std::endl;
					break;
				}
			}
		}
	}
}

bool SFMLServerHandler::ConnectToServer()
{
	m_socket = std::make_shared<sf::TcpSocket>();

	if (m_socket->connect(m_serverAddress, m_port) != sf::Socket::Status::Done)
	{
		m_socket.reset();

		std::cerr << "Error connecting to server\n";
		return false;
	}

	return true;
}

bool SFMLServerHandler::DisconnectFromServer()
{
	if (m_socket)
	{
		m_socket->disconnect();
		return true;
	}

	return false;
}
