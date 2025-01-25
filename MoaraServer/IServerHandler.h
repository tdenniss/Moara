#pragma once
#include <string>
#include <functional>

using ServerHandlerPtr = class IServerHandler*;

using DataReceivedCallback = std::function<void(const void* data, int size, int clientId)>;
using DisconnectedCallback = std::function<void(int clientId)>;

class IServerHandler
{
public:
	virtual bool StartServer(unsigned short port,
							DataReceivedCallback dataCallback,
							DisconnectedCallback disconnectedCallback) = 0;

	virtual void SendData(const void* data, int dataSize, int clientId) = 0;

	virtual void SetDataReceivedCallback(DataReceivedCallback callback) = 0;
	virtual void SetDisconnectedCallback(DisconnectedCallback callback) = 0;

	virtual ~IServerHandler() = default;
};