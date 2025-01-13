#include "SDK.h"

IClientSDKPtr IClientSDK::Create(std::string ip, int port)
{
	return std::make_shared<SDK>(ip, port);
}
