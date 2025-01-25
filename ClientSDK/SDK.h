#pragma once
#include "GameConfig.h"
#include "IClientSDK.h"
#include "IServerHandler.h"

using Command = std::function<void(void* data)>;
using CommandMap = std::unordered_map <std::string, Command>;

class SDK : public IClientSDK
{
public:
	SDK() = default;
	SDK(std::string ip, int port, IServerHandlerPtr serverHandler);
	~SDK();

	void SetListener(IClientSDKListener* listener) override;

	void PlacePiece(uint8_t index) override;
	void RemovePiece(uint8_t index) override;
	void MovePiece(uint8_t from, uint8_t to) override;

	void StartGame(GameConfigPtr config) override;

	void SetupBoard() override;

	void CreateLobby() override;
	void JoinLobby(int id) override;

	void ChangeConfig(int boardSize, int boardType, EComputerLevel level) override;

	void Undo() override;

	void GetTimer() override;
	void GetPossibleMoves(uint8_t index) override;
	void GetPossibleRemoves() override;

	void GetWinnigPlayer() override;
	void GetActivePlayer() override;

private:
	void OnNewData(void* data);
	void OnServerDisconnected();

	void InitializeCommands();

private:
	IServerHandlerPtr m_serverHandler;

	IClientSDKListener* m_listener;

	CommandMap m_commands;
};

