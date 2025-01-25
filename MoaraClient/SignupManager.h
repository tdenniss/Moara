#pragma once

#include <QFrame>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include "IClientSDKListener.h"
#include "IClientSDK.h"

class SignupManager : public QFrame, public IClientSDKListener
{
	Q_OBJECT

public:
	SignupManager(QWidget* parent = nullptr);
	~SignupManager() = default;

public: // IClientSDKListener
	void OnSignUpSuccess() override;
	void OnError(const std::string& message) override;

	void OnLoginSuccess() override {};
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

signals:
	void GoToLogIn(IClientSDKPtr sdk = nullptr);

public slots:
	void OnGoToSignUp(IClientSDKPtr sdk);

	void showEvent(QShowEvent* event) override;

private:
	void OnSignUpCredentialsSent() noexcept;
	void OnTogglePasswordView() noexcept;

private:
	IClientSDKPtr m_sdk;

	bool m_firstShow;
	QLineEdit* m_nameInput;
	QLineEdit* m_passwordInput;

	QPushButton* m_signUpButton;
	QPushButton* m_togglePasswordView;
	QPushButton* m_goToLogInButton;

	QLabel* m_alreadyExistingAccountLabel;
	QString m_incorrectCredentialsText;

	QIcon m_viewPasswordIcon;
	QIcon m_hidePasswordIcon;
};
