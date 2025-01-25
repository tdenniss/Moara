#include "SDK.h"

#include "SFMLServerHandler.h"
#include "QtServerHandler.h"

IClientSDKPtr IClientSDK::Create(std::string ip, int port, HandlerType handlerType)
{
	switch (handlerType)
	{
	case HandlerType::SFML:
		return std::make_shared<SDK>(ip, port, std::make_shared<SFMLServerHandler>());
	case HandlerType::QtWebSockets:
		return std::make_shared<SDK>(ip, port, std::make_shared<QtServerHandler>());
	default:
		return{};
	}
}
