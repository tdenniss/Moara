#pragma once

#include "EBoardSize.h"
#include "EBoardType.h"
#include "EGameState.h"
#include "EComputerLevel.h"

#include "IClientSDK.h"

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMetaObject>

#include <vector>

using ButtonGroup = std::pair<QPushButton*, int>;
using UiButtons = std::vector<ButtonGroup>;

Q_DECLARE_METATYPE(NodesInfo)

class MenuScene : public QWidget, public IClientSDKListener
{
	Q_OBJECT

public:
	MenuScene(QWidget* parent = nullptr);
	~MenuScene();

public: // IClientSDKListener
	void OnGameStarted() override;
	void OnJoinedLobby(int lobbyId) override;
	void OnCreatedLobby(int lobbyId) override;
	void OnError(const std::string& message) override;
	void OnInfo(const std::string& message) override;
	void OnSetupBoard(NodesInfo nodesInfo) override;
	void OnChangedConfig(EBoardType type, EBoardSize size, EComputerLevel level) override;
	void OnPlayerLeft() override;
	void OnUpdateTimer(int timeRemaining) override {}
	void OnWinningPlayer(const std::string& player) override {}
	void OnAddedPiece(int index, EPieceType pieceType) override {}
	void OnMovedPiece(int from, int to, EPieceType pieceType) override {}
	void OnRemovedPiece(int index) override {}
	void OnPlayerRemoved(const std::string& player) override {}
	void OnPlayerChanged(const std::string& player, bool isComputer) override {}
	void OnStateChanged(EGameState state) override {}
	void OnWindmill() override {}
	void OnPossibleMoves(PiecesIndexes indexes) override {}
	void OnPossibleRemoves(PiecesIndexes indexes) override {}
	void OnServerDisconnect() override;

signals:
	void SceneChange(IClientSDKPtr sdk, EBoardType type, EBoardSize size, NodesInfo nodeInfoList);

public slots:
	void OnStartGame();
	void OnTypeSelected();
	void OnSizeSelected();
	void OnPlayerSelected();
	void OnPaintButtons();
	void OnJoinLobby();
	void OnCreateLobby();

	void SetupWindow(NodesInfo nodesInfoList);
	void ShowMessage(const std::string& message);
	void SetLabelText(QLabel* label, const QString& text);
	void ChangeButtonColor(QPushButton* button);

protected:
	void showEvent(QShowEvent* event) override;

private:
	void InitGraphics();

	void SetPvpConfig(int numberOfPlayers, int boardConfiguration);
	void SetAiPlayers(EComputerLevel level);
	void CheckAndSetAiPlayers(QRegularExpression regex, EComputerLevel level);

	int GetButtonGroup(QPushButton* button);

	template<typename... Slots>
	void ConnectButton(QPushButton* button, int group, Slots... uiSlots);
	void ConnectAllButtons();
private:
	const std::string SERVER_ADDRESS = "localhost";
	const unsigned short PORT = 53000;

	bool m_firstShow;

	EBoardType m_type;
	EBoardSize m_size;
	EComputerLevel m_level;
	GameConfigPtr m_config;

	UiButtons m_buttons;

	IClientSDKPtr m_sdk;

	QPushButton* m_normalSizeButton;
	QPushButton* m_bigSizeButton;
	QPushButton* m_normalTypeButton;
	QPushButton* m_diagonalTypeButton;
	QPushButton* m_startGameButton;
	QPushButton* m_easyGame;
	QPushButton* m_mediumGame;
	QPushButton* m_hardGame;
	QPushButton* m_pvpButton;
	QPushButton* m_joinLobby;
	QPushButton* m_createLobby;

	QLineEdit* m_numberOfAI;
	QLineEdit* m_lobbyIdInput;

	QLabel* m_levelLabel;
	QLabel* m_typeLabel;
	QLabel* m_sizeLabel;
	QLabel* m_playersLabel;
	QLabel* m_createdLobbyIdLabel;
};

