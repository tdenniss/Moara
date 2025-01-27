#pragma once

#include "IClientSDK.h"
#include "IClientSDKListener.h"
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <string>

class LoginManager : public QFrame, public IClientSDKListener
{
	Q_OBJECT

public:
	LoginManager(QWidget* parent = nullptr);
	~LoginManager();

public: // IClientSDKListener
	void OnLoginSuccess() override;
	void OnError(const std::string& message) override;

	void OnSignUpSuccess() override {};
	void OnGameStarted() override {};
	void OnJoinedLobby(int lobbyId) override {};
	void OnCreatedLobby(int lobbyId) override {};
	void OnInfo(const std::string& message) override {};
	void OnSetupBoard(NodesInfo nodesInfo) override {};
	void OnChangedConfig(EBoardType type, EBoardSize size, EComputerLevel level) override {};
	void OnPlayerLeft() override {};
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
	void OnServerDisconnect() override {};

public slots:
	void OnGoToLogin(IClientSDKPtr sdk);

	void showEvent(QShowEvent* event) override;

signals:
	void GoToSignUpScene(IClientSDKPtr sdk);
	void GoToMenu(IClientSDKPtr sdk);

private slots:
	void OnLogInCredentialsSent() noexcept;
	void OnTogglePasswordView() noexcept;

private:
	const std::string SERVER_ADDRESS = "localhost";
	const unsigned short PORT = 12345;

	IClientSDKPtr m_sdk;

	bool m_firstShow;
	QLineEdit* m_nameInput;
	QLineEdit* m_passwordInput;

	QPushButton* m_logInButton;
	QPushButton* m_togglePasswordView;
	QPushButton* m_signupButton;

	QIcon m_viewPasswordIcon;
	QIcon m_hidePasswordIcon;

	QLabel* m_incorrectCredentialsLabel;
};
