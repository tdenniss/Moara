#pragma once
#include "IGameConfig.h"
#include "EPieceType.h"
#include "HandlerType.h"
#include "IClientSDKListener.h"

#include <functional>

using NodesInfo = std::vector<std::pair<int, EPieceType>>;

using IClientSDKPtr = std::shared_ptr<class IClientSDK>;

class IClientSDK
{
public:
	static IClientSDKPtr Create(std::string ip, int port, HandlerType handlerType);

	virtual void SetListener(IClientSDKListener* listener) = 0;

	//requests
	virtual void SignUp(std::string username, std::string password) = 0;
	virtual void Login(std::string username, std::string password) = 0;

	virtual void CreateLobby() = 0;
	virtual void JoinLobby(int id) = 0;

	virtual void ChangeConfig(int boardSize, int boardType, EComputerLevel level) = 0;

	virtual void StartGame(GameConfigPtr config) = 0;

	virtual void SetupBoard() = 0;

	virtual void PlacePiece(uint8_t index) = 0;
	virtual void RemovePiece(uint8_t index) = 0;
	virtual void MovePiece(uint8_t from, uint8_t to) = 0;
	virtual void Undo() = 0;

	virtual void GetTimer() = 0;
	virtual void GetPossibleMoves(uint8_t index) = 0;
	virtual void GetPossibleRemoves() = 0;
	virtual void GetWinnigPlayer() = 0;
	virtual void GetActivePlayer() = 0;

	virtual void ExitGame() = 0;
};
