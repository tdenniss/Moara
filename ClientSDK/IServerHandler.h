#pragma once

#include <string>
#include <memory>
#include <functional>

using NewDataCallback = std::function<void(void*)>;
using DisconnectedCallback = std::function<void()>;

using IServerHandlerPtr = std::shared_ptr<class IServerHandler>;

class IServerHandler
{
public:
	virtual bool Start(const std::string& host, int port) = 0;
	virtual void Stop() = 0;

	virtual void SendRequest(const void* message, int size) = 0;

	virtual void SetNewDataCallback(NewDataCallback newDataFunction) = 0;
	virtual void SetDisconnectedCallback(DisconnectedCallback newDataFunction) = 0;

	virtual ~IServerHandler() = default;
};