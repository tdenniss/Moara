#include "JsonMessageHandler.h"
#include "EnumStringMaker.h"

#include "MenuScene.h"

#include "Stylesheets.h"

#include <QRegularExpression>
#include <QMessageBox>

#include <ranges>
#include <algorithm>
#include "IClientSDK.h"
#include "SDK.h"

MenuScene::MenuScene(QWidget* parent)
	: QWidget(parent)
	, m_type(EBoardType::Normal)
	, m_size(EBoardSize::Normal)
	, m_level(EComputerLevel::Easy)
	, m_config()
{
	m_firstShow = true;
	
	m_sdk = IClientSDK::Create(SERVER_ADDRESS, PORT, HandlerType::QtWebSockets);
	m_sdk->SetListener(this);
	qRegisterMetaType<NodesInfo>("NodesInfo");
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	m_config = std::make_shared<GameConfig>();
}

MenuScene::~MenuScene()
{
}

void MenuScene::ConnectAllButtons()
{
	ConnectButton(m_normalSizeButton, 0, &MenuScene::OnSizeSelected, &MenuScene::OnPaintButtons);
	ConnectButton(m_bigSizeButton, 0, &MenuScene::OnSizeSelected, &MenuScene::OnPaintButtons);
	ConnectButton(m_normalTypeButton, 1, &MenuScene::OnTypeSelected, &MenuScene::OnPaintButtons);
	ConnectButton(m_diagonalTypeButton, 1, &MenuScene::OnTypeSelected, &MenuScene::OnPaintButtons);
	ConnectButton(m_startGameButton, -1, &MenuScene::OnStartGame);
	ConnectButton(m_easyGame, 2, &MenuScene::OnPlayerSelected, &MenuScene::OnPaintButtons);
	ConnectButton(m_mediumGame, 2, &MenuScene::OnPlayerSelected, &MenuScene::OnPaintButtons);
	ConnectButton(m_hardGame, 2, &MenuScene::OnPlayerSelected, &MenuScene::OnPaintButtons);
	ConnectButton(m_pvpButton, 2, &MenuScene::OnPlayerSelected, &MenuScene::OnPaintButtons);
	ConnectButton(m_joinLobby, -1, &MenuScene::OnJoinLobby);
	ConnectButton(m_createLobby, -1, &MenuScene::OnCreateLobby);
}

void MenuScene::ResetScene()
{
	m_type = EBoardType::Normal;
	m_size = EBoardSize::Normal;
	m_level = EComputerLevel::Easy;
}

void MenuScene::InitGraphics()
{
	this->setMinimumHeight(700);
	this->setMinimumWidth(700);

	m_normalSizeButton->setStyleSheet(pushedButtonStyle);
	m_normalTypeButton->setStyleSheet(pushedButtonStyle);
	m_easyGame->setStyleSheet(pushedButtonStyle);

	m_numberOfAI->setText(QString::fromStdString("1"));

	m_levelLabel->setText(QString::fromStdString("AI Level: Easy"));
	m_typeLabel->setText(QString::fromStdString("Board Type: Normal"));
	m_sizeLabel->setText(QString::fromStdString("Board Size: Normal"));
	m_playersLabel->setText(QString::fromStdString("AI Count: 1"));

}

template<typename... Slots>
void MenuScene::ConnectButton(QPushButton* button, int group, Slots... uiSlots)
{
	bool found = false;

	for (auto& storedButton : m_buttons)
	{
		if (storedButton.first == button)
		{
			found = true;
			break;
		}
	}

	if (!found)
		m_buttons.emplace_back(button, group);

	(connect(button, &QPushButton::clicked, this, uiSlots), ...);
}

void MenuScene::OnTypeSelected()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());

	if (button) {
		QString buttonText = button->text();

		if (buttonText == "Normal")
		{
			m_type = EBoardType::Normal;
			m_typeLabel->setText(QString::fromStdString("Board Type: Normal"));
		}
		else
		{
			m_type = EBoardType::Diagonals;
			m_typeLabel->setText(QString::fromStdString("Board Type: Diagonals"));
		}
	}

	m_sdk->ChangeConfig(static_cast<int>(m_size), static_cast<int>(m_type), m_config->GetComputerLevel());
}

void MenuScene::OnSizeSelected()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());

	if (button) {
		QString buttonText = button->text();

		if (buttonText == "Normal")
		{
			m_size = EBoardSize::Normal;
			m_sizeLabel->setText(QString::fromStdString("Board Size: Normal"));
		}
		else
		{
			m_size = EBoardSize::Big;
			m_sizeLabel->setText(QString::fromStdString("Board Size: Big"));
		}
	}
	m_sdk->ChangeConfig(static_cast<int>(m_size), static_cast<int>(m_type), m_config->GetComputerLevel());
}

void MenuScene::OnPlayerSelected()
{
	QPushButton* button = qobject_cast<QPushButton*>(sender());

	if (button)
	{
		QString buttonText = button->text();
		QRegularExpression regex("^\\d+$");

		if (buttonText == "Easy Game")
		{
			CheckAndSetAiPlayers(regex, EComputerLevel::Easy);

			m_levelLabel->setText(QString::fromStdString("AI Level: Easy"));

			m_level = EComputerLevel::Easy;
		}
		else if (buttonText == "Medium Game")
		{
			CheckAndSetAiPlayers(regex, EComputerLevel::Medium);

			m_levelLabel->setText(QString::fromStdString("AI Level: Medium"));

			m_level = EComputerLevel::Medium;
		}
		else if (buttonText == "Hard Game")
		{
			CheckAndSetAiPlayers(regex, EComputerLevel::Hard);

			m_levelLabel->setText(QString::fromStdString("AI Level: Hard"));
			m_level = EComputerLevel::Hard;
		}
		else if (buttonText == "PvP")
		{

			if (m_size == EBoardSize::Normal)
			{
				SetPvpConfig(2, 0);
			}
			else if (m_size == EBoardSize::Big)
			{
				SetPvpConfig(3, 1);
			}
			else
			{
				ShowMessage("First select board size");
				return;
			}

			m_levelLabel->setText(QString::fromStdString("PvP"));
		}

		m_sdk->ChangeConfig(static_cast<int>(m_size), static_cast<int>(m_type), m_level);
	}
}

void MenuScene::OnPaintButtons()
{
	auto senderButton = qobject_cast<QPushButton*>(sender());
	if (senderButton)
	{
		for (auto button : m_buttons)
		{
			if (button.first == senderButton)
			{
				button.first->setStyleSheet(pushedButtonStyle);
			}
			else if (button.first != senderButton && button.second == GetButtonGroup(senderButton))
			{
				button.first->setStyleSheet(buttonStyle);
			}
			button.first->update();
		}
	}
}

void MenuScene::OnJoinLobby()
{
	QRegularExpression regex("^\\d+$");

	QString lobbyIdString = m_lobbyIdInput->text();

	if (regex.match(lobbyIdString).hasMatch())
		m_sdk->JoinLobby(lobbyIdString.toInt());
	else
		ShowMessage("Insert a number for lobby id");

}

void MenuScene::OnCreateLobby()
{
	m_sdk->CreateLobby();
}

void MenuScene::SetupWindow(NodesInfo nodeInfoList)
{
	emit SceneChange(m_sdk, m_type, m_size, nodeInfoList);
	ResetScene();
}

void MenuScene::SetPvpConfig(int numberOfPlayers, int boardConfiguration)
{
	m_config->SetNumberOfPlayers(numberOfPlayers);
	m_config->SetPlayersConfig(humanBoardConfiguration, boardConfiguration);
}

void MenuScene::CheckAndSetAiPlayers(QRegularExpression regex, EComputerLevel level)
{
	if (regex.match(m_numberOfAI->text()).hasMatch())
	{
		SetAiPlayers(level);

		m_playersLabel->setText(QString::fromStdString("AI Count: ") + m_numberOfAI->text());
	}
	else
	{
		ShowMessage("Please insert a number of AI players");
	}
}

int MenuScene::GetButtonGroup(QPushButton* button)
{
	for (auto [storedButton, group] : m_buttons)
	{
		if (button == storedButton)
			return group;
	}
	return -1;
}

void MenuScene::ChangeButtonColor(QPushButton* button)
{
	auto buttonGroup = GetButtonGroup(button);

	button->setStyleSheet(pushedButtonStyle);

	for (auto& buttonPair : m_buttons)
	{
		if (buttonPair.second == buttonGroup && buttonPair.first != button)
			buttonPair.first->setStyleSheet(buttonStyle);
	}
}

void MenuScene::showEvent(QShowEvent* event)
{
	if (m_firstShow) {
		m_normalSizeButton = findChild<QPushButton*>("normalSizeButton");
		m_bigSizeButton = findChild<QPushButton*>("bigSizeButton");
		m_normalTypeButton = findChild<QPushButton*>("normalTypeButton");
		m_diagonalTypeButton = findChild<QPushButton*>("diagonalTypeButton");
		m_startGameButton = findChild<QPushButton*>("startGameButton");
		m_easyGame = findChild<QPushButton*>("easyGame");
		m_mediumGame = findChild<QPushButton*>("mediumGame");
		m_hardGame = findChild<QPushButton*>("hardGame");
		m_pvpButton = findChild<QPushButton*>("pvpButton");
		m_joinLobby = findChild<QPushButton*>("joinLobby");
		m_createLobby = findChild<QPushButton*>("createLobby");
		m_numberOfAI = findChild<QLineEdit*>("numberOfAI");
		m_levelLabel = findChild<QLabel*>("levelLabel");
		m_typeLabel = findChild<QLabel*>("typeLabel");
		m_sizeLabel = findChild<QLabel*>("sizeLabel");
		m_playersLabel = findChild<QLabel*>("playersLabel");
		m_createdLobbyIdLabel = findChild<QLabel*>("createdLobbyIdLabel");
		m_lobbyIdInput = findChild<QLineEdit*>("lobbyIdInput");

		InitGraphics();
		ConnectAllButtons();

		m_firstShow = false;
	}
}

void MenuScene::ShowMessage(const std::string& message)
{
	auto messageBox = new QMessageBox(this);
	messageBox->setText(QString::fromStdString(message));
	messageBox->setIcon(QMessageBox::Information);
	messageBox->setStyleSheet("background-color: rgb(255, 255, 255);");
	messageBox->show();
}

void MenuScene::SetLabelText(QLabel* label, const QString& text)
{
	if (text[0] == 'J')
	{
		auto sp = m_startGameButton->sizePolicy();
		sp.setRetainSizeWhenHidden(true);

		m_startGameButton->setSizePolicy(sp);
		m_startGameButton->hide();
	}

	label->setText(text);
}

void MenuScene::SetAiPlayers(EComputerLevel level)
{
	bool ok;
	int numberOfAI = m_numberOfAI->text().toInt(&ok);

	if (m_size == EBoardSize::Normal)
	{
		m_config->SetNumberOfPlayers(2);

		if (numberOfAI < 1 || numberOfAI > 2)
		{
			ShowMessage("The number of AI players shouldn't pass 2 or be smaller than 1");
		}
		else
		{
			if (ok)
				m_config->SetPlayersConfig(smallAIBoardConfiguration, numberOfAI - 1);

		}
	}
	else if (m_size == EBoardSize::Big)
	{
		m_config->SetNumberOfPlayers(3);

		if (numberOfAI < 1 || numberOfAI > 3)
		{
			ShowMessage("The number of AI players shouldn't pass 3 or be smaller than 1");
		}
		else
		{
			if (ok)
				m_config->SetPlayersConfig(bigAIBoardConfiguration, numberOfAI - 1);
		}
	}

	m_config->SetComputerLevel(level);
}

void MenuScene::OnGameStarted()
{
	m_sdk->SetupBoard();
}

void MenuScene::OnJoinedLobby(int lobbyId)
{
	QMetaObject::invokeMethod(this, "SetLabelText", Qt::QueuedConnection,
		Q_ARG(QLabel*, m_createdLobbyIdLabel),
		Q_ARG(QString, QString::fromStdString("Joined lobby id: " + std::to_string(lobbyId))));
}

void MenuScene::OnCreatedLobby(int lobbyId)
{
	QMetaObject::invokeMethod(this, "SetLabelText", Qt::QueuedConnection,
		Q_ARG(QLabel*, m_createdLobbyIdLabel),
		Q_ARG(QString, QString::fromStdString("Created lobby id: " + std::to_string(lobbyId))));
}

void MenuScene::OnError(const std::string& message)
{
	QMetaObject::invokeMethod(this, "ShowMessage", Qt::QueuedConnection
		, Q_ARG(std::string, message));
}

void MenuScene::OnInfo(const std::string& message)
{

}

void MenuScene::OnSetupBoard(NodesInfo nodesInfo)
{
	QMetaObject::invokeMethod(this, "SetupWindow", Qt::QueuedConnection, Q_ARG(NodesInfo, nodesInfo));
}

void MenuScene::OnChangedConfig(EBoardType type, EBoardSize size, EComputerLevel level)
{
	switch (static_cast<EBoardSize>(size))
	{
	case EBoardSize::Normal:
		m_size = EBoardSize::Normal;

		QMetaObject::invokeMethod(this, "ChangeButtonColor", Qt::QueuedConnection, Q_ARG(QPushButton*, m_normalSizeButton));
		m_sizeLabel->setText(QString::fromStdString("Board Size: Normal"));
		break;
	case EBoardSize::Big:
		m_size = EBoardSize::Big;

		QMetaObject::invokeMethod(this, "ChangeButtonColor", Qt::QueuedConnection, Q_ARG(QPushButton*, m_bigSizeButton));
		m_sizeLabel->setText(QString::fromStdString("Board Size: Big"));
		break;
	default:
		break;
	}

	switch (static_cast<EBoardType>(type))
	{
	case EBoardType::Normal:
		m_type = EBoardType::Normal;

		QMetaObject::invokeMethod(this, "ChangeButtonColor", Qt::QueuedConnection, Q_ARG(QPushButton*, m_normalTypeButton));
		m_typeLabel->setText(QString::fromStdString("Board Type: Normal"));
		break;
	case EBoardType::Diagonals:
		m_type = EBoardType::Diagonals;

		QMetaObject::invokeMethod(this, "ChangeButtonColor", Qt::QueuedConnection, Q_ARG(QPushButton*, m_diagonalTypeButton));
		m_typeLabel->setText(QString::fromStdString("Board Type: Diagonals"));
		break;
	default:
		break;
	}

	switch (level)
	{
	case EComputerLevel::Easy:
		QMetaObject::invokeMethod(this, "ChangeButtonColor", Qt::QueuedConnection, Q_ARG(QPushButton*, m_easyGame));
		m_levelLabel->setText(QString::fromStdString("AI Level: Easy"));
		break;
	case EComputerLevel::Medium:
		QMetaObject::invokeMethod(this, "ChangeButtonColor", Qt::QueuedConnection, Q_ARG(QPushButton*, m_mediumGame));
		m_levelLabel->setText(QString::fromStdString("AI Level: Medium"));
		break;
	case EComputerLevel::Hard:
		QMetaObject::invokeMethod(this, "ChangeButtonColor", Qt::QueuedConnection, Q_ARG(QPushButton*, m_hardGame));
		m_levelLabel->setText(QString::fromStdString("AI Level: Hard"));
		break;
	case EComputerLevel::None:
		QMetaObject::invokeMethod(this, "ChangeButtonColor", Qt::QueuedConnection, Q_ARG(QPushButton*, m_pvpButton));
		m_levelLabel->setText(QString::fromStdString("PvP"));
		break;
	default:
		break;
	}
}

void MenuScene::OnPlayerLeft()
{
	QMetaObject::invokeMethod(this, "ShowMessage", Qt::QueuedConnection, Q_ARG(std::string, "A player has left the lobby"));
}

void MenuScene::OnServerDisconnect()
{
	QMetaObject::invokeMethod(this, "ShowMessage", Qt::QueuedConnection
		, Q_ARG(std::string, "lost connection to server, please restart"));
}

void MenuScene::OnStartGame()
{
	m_config->SetBoardConfig(m_type, m_size);

	m_sdk->StartGame(m_config);
}