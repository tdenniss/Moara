#pragma once
#include "IServerHandler.h"

#include <SFML/Network.hpp>

#include <atomic>
#include <memory>

using SFMLServerHandlerPtr = std::shared_ptr<class SFMLServerHandler>;

class SFMLServerHandler : public IServerHandler
{
public:
	SFMLServerHandler();
	~SFMLServerHandler();

	bool Start(const std::string& host, int port) override;
	void Stop() override;

	void SendRequest(const void* message, int size) override;

	void SetNewDataCallback(NewDataCallback newDataFunction) override;
	void SetDisconnectedCallback(DisconnectedCallback newDataFunction) override;

private:
	bool ConnectToServer();
	bool DisconnectFromServer();

	void Listen();

private:
	std::string m_serverAddress;
	unsigned short m_port;

	std::shared_ptr<sf::TcpSocket> m_socket;

	sf::Thread m_thread;
	std::atomic<bool> m_running;

	NewDataCallback m_newDataFunction;
	DisconnectedCallback m_disconnectedFunction;
};
