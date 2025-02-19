#pragma once

#include "IUiBoard.h"

#include "IClientSDK.h"

#include <QWidget>
#include <QPaintEvent>
#include <QMessageBox>

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <QVBoxLayout>

class GameScene : public QWidget, public IClientSDKListener
{
	Q_OBJECT

public:
	GameScene(QWidget* parent = nullptr);

	~GameScene();

	// IClientSDKListener
	void OnSignUpSuccess() override {};
	void OnLoginSuccess() override {};
	void OnGameStarted() override {}
	void OnJoinedLobby(int lobbyId) override {}
	void OnCreatedLobby(int lobbyId) override {}
	void OnError(const std::string& message) override;
	void OnInfo(const std::string& message) override;
	void OnSetupBoard(NodesInfo nodesInfo) override;
	void OnChangedConfig(EBoardType type, EBoardSize size, EComputerLevel level) override {}
	void OnPlayerLeft() override;
	void OnUpdateTimer(int timeRemaining) override;
	void OnWinningPlayer(const std::string& player) override;
	void OnAddedPiece(int index, EPieceType pieceType) override;
	void OnMovedPiece(int from, int to, EPieceType pieceType) override;
	void OnRemovedPiece(int index) override;
	void OnPlayerRemoved(const std::string& player) override;
	void OnPlayerChanged(const std::string& player, bool isComputer) override;
	void OnStateChanged(EGameState state) override;
	void OnWindmill() override;
	void OnPossibleMoves(PiecesIndexes indexes) override;
	void OnPossibleRemoves(PiecesIndexes indexes) override;
	void OnServerDisconnect() override;

	// own methods
	void SetBoard(IUiBoard* board);

signals:
	void LeaveGame();

protected:
	void showEvent(QShowEvent* event) override;

private:
	void InitLayouts();
	void InitWidgets();

	void SetLabels(EPieceType playerType, bool isComputer);
	void CustomizeLabels(QLabel* label);

	void ConnectMethods();

	void ResetScene();

public slots:
	void OnSceneChange(IClientSDKPtr sdk, EBoardType type, EBoardSize size, NodesInfo nodeInfoList);
	void UpdateBoard(NodesInfo nodeInfoList);

	void ShowMessage(const QString& message);
	void ShowErrorMessage(const std::string& error);
	void ShowStartingWindow();

	void OnNodeClicked(uint8_t index);
	void OnRemoveNodeClicked(uint8_t index);
	void OnMove(uint8_t fromIndex, uint8_t toIndex);
	void OnUndo();
	void OnLeaveGame();

	void OnHighlightMove(uint8_t index);
	void OnHighlightRemove();

private slots:
	void UpdateTimer();

private:
	IUiBoard* m_board;

	QTimer* m_timer;
	QLabel* m_errorLabel;
	QLabel* m_timerLabel;
	QLabel* m_movingPlayerLabel;
	QLabel* m_computerThinking;
	QPushButton* m_undoButton;
	QPushButton* m_exitButton;

	QWidget* m_gameWidget;

	IClientSDKPtr m_sdk;

	EGameState m_gameState;

	bool m_firstShow;
};
