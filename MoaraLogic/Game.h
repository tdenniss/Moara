#pragma once
#include "IGame.h"
#include "IPlayer.h"

#include "BoardFactory.h"
#include "Command.h"

#include <vector>
#include <array>
#include <memory>
#include <functional>
#include <ranges>
#include <algorithm>

using IsComputer = bool;
using PlayerPtrList = std::vector<PlayerPtr>;
using Listeners = std::vector<ListenerWeakPtr>;
using NotifyFunction = std::function<void(IGameListener*)>;
using Commands = std::vector<std::shared_ptr<Command>>;

class Game : public IGame
{
public:
	Game();

	~Game()
	{
		for (auto& player : m_players)
			player.~shared_ptr();
	}

	//IGame
	void Initialize(GameConfigPtr& gameConfig) override final;

	EGameState GetGameState()							const override final;

	EPieceType GetWinningPlayer()						const override final;
	EPieceType GetActivePlayer()						const override final;

	EPieceType GetPiece(uint8_t nodeIndex)				const override final;
	PieceTypeList GetAllPieces()						const override final;

	EPieceType GetNode(uint8_t nodeIndex)				const override final;
	NodeList GetAllNodes()								const override final;

	PlayerTypeList GetAllPlayerTypes()					const override final;
	uint8_t	GetPlayerPiecesOnBoard(EPieceType player)	const override final;
	uint8_t	GetPlayerPiecesToPlace(EPieceType player)	const override final;

	PieceIndexes GetPossibleMovesFromNode(uint8_t nodeIndex)	const override final;
	PieceIndexes GetPossibleRemoves()					const override final;

	void SetBoard(IBoardPtr board);

	void CheckWiningPlayer();
	void CheckBlocks();

	void AddListener(ListenerSharedPtr listener) override;
	void RemoveListener(ListenerWeakPtr listener) override;
	void RemoveAllListeners() override;

	//IGameControls
	void PlacePiece(uint8_t nodeIndex)					override final;
	void MovePiece(uint8_t fromIndex, uint8_t toIndex)	override final;
	void RemovePiece(uint8_t nodeIndex)					override final;
	void Undo()											override final;

	void LoadGame(const std::string& filePath) override final;
	void SaveGame(const std::string& filePath) const override final;

	void SetComputerLevel(ComputerLevelPtr computerLevel) override;
	void SetComputerLevel(EComputerLevel level) override;
	void SetPlayerConfig(PlayerConfig& config) override;

	void LetComputerPlay() override;
	bool IsComputerTurn() override;

	size_t HowManyListeners();
	void SetGameState(EGameState state);

	int GetActivePlayerIndex() const override;

private:
	void InitLevel(EComputerLevel level);
	void InitPlayers(const Config& playersConfig);
	void InitBoard(const BoardConfigMatrix& boardMatrix, EBoardType type, int piecesToPlace);

	void NextPlayer();
	void PrevPlayer();

	void NotifyAll(NotifyFunction func);

	void UndoPlacePiece(uint8_t nodeIndex);
	void UndoMovePiece(uint8_t fromIndex, uint8_t toIndex);
	void UndoRemovePiece(uint8_t nodeIndex);

	NotifyFunction GetNotifyAddedPiece(uint8_t index, EPieceType type);
	NotifyFunction GetNotifyGameStateChanged(EGameState newState);
	NotifyFunction GetNotifyRemovedPiece(uint8_t index);
	NotifyFunction GetNotifyMovedPiece(uint8_t fromIndex, uint8_t toIndex, EPieceType movedPiceType);
	NotifyFunction GetNotifyWindmill();
	NotifyFunction GetNotifyPlayerChanged(EPieceType playerType, bool isComputer);
	NotifyFunction GetNotifyPlayerRemoved(EPieceType who);

private:
	Listeners		m_listeners;
	PlayerPtrList	m_players;
	IBoardPtr		m_board;
	EGameState		m_state;
	Commands		m_moves;
	EPlayerType		m_winner;
	ComputerLevelPtr m_computer;

	int				m_activePlayer;

	friend class Command;
	friend class Move;
	friend class Place;
	friend class Remove;
	friend class RemovePlayer;
};