#include "EnumStringMaker.h"
#include "JsonMessageHandler.h"

#include "GameScene.h"
#include "UiBoard.h"

#include "Stylesheets.h"

#include <QLabel>
#include <QTimer>
#include <QHBoxLayout>
#include <QDebug>

#include <functional>

GameScene::GameScene(QWidget* parent)
	: QWidget(parent)
	, m_board(nullptr)
	, m_movingPlayerLabel(new QLabel(""))
	, m_errorLabel(new QLabel(""))
	, m_undoButton(new QPushButton(this))
	, m_timerLabel(new QLabel("0"))
	, m_computerThinking(new QLabel(""))
	, m_firstShow(true)
{
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

GameScene::~GameScene()
{
	delete m_board;
	delete m_errorLabel;
	delete m_movingPlayerLabel;
	delete m_undoButton;
	delete m_computerThinking;
}

void GameScene::OnError(const std::string& message)
{
	ShowErrorMessage(message);
}

void GameScene::OnInfo(const std::string& message)
{
	ShowMessage(QString::fromStdString(message));
}

void GameScene::OnSetupBoard(NodesInfo nodesInfo)
{
	QMetaObject::invokeMethod(this, "SetupWindow", Qt::QueuedConnection, Q_ARG(NodesInfo, nodesInfo));
}

void GameScene::OnPlayerLeft()
{
	QMetaObject::invokeMethod(this, "ShowMessage", Qt::QueuedConnection, Q_ARG(QString, "A player has left the lobby"));
	QMetaObject::invokeMethod(this, "ShowStartingWindow", Qt::QueuedConnection);
}

void GameScene::OnUpdateTimer(int timeRemaining)
{
	m_timerLabel->setText(QString::number(timeRemaining));
}

void GameScene::OnWinningPlayer(const std::string& player)
{
	std::string message = "Game finished. The winning player is " + player;

	QMetaObject::invokeMethod(this, "ShowMessage", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(message)));
}

void GameScene::OnAddedPiece(int index, EPieceType pieceType)
{
	m_board->AddPiece(index, pieceType);
}

void GameScene::OnMovedPiece(int from, int to, EPieceType pieceType)
{
	m_board->ResetNodesHighlight();
	m_board->ResetComputerNodesHighlight();

	m_board->SwapPieces(from, to, pieceType);

	auto buttons = m_board->GetAllButtons();

	buttons[from]->Highlight(QPen(Qt::yellow, 5));
	buttons[to]->Highlight(QPen(Qt::green, 5));

	m_board->SetHighlightedNodeIndex(from);
	m_board->SetHighlightedNodeIndex(to);
}

void GameScene::OnRemovedPiece(int index)
{
	m_board->RemovePiece(index);

	m_board->ResetComputerNodesHighlight();
}

void GameScene::OnPlayerRemoved(const std::string& player)
{
	QString message = QString::fromStdString(player + " got removed.");
	QMetaObject::invokeMethod(this, "ShowMessage", Qt::QueuedConnection, Q_ARG(QString, message));

	m_sdk->SetupBoard();
}

void GameScene::OnPlayerChanged(const std::string& player, bool isComputer)
{
	m_board->ResetNodesHighlight();

	SetLabels(EPieceTypeStringMaker::GetEnumFromString(player), isComputer);
}

void GameScene::OnStateChanged(EGameState state)
{
	std::string message;

	switch (state)
	{
	case EGameState::Placing:
		m_gameState = EGameState::Placing;
		message = "Keep placing.";
		break;
	case EGameState::Moving:
		m_gameState = EGameState::Moving;
		message = "Moving phase, select which node to move and where to move it.";
		break;
	case EGameState::Removing:
		m_gameState = EGameState::Removing;
		message = "Removing phase, select which node to remove.";
		OnHighlightRemove();
		break;
	case EGameState::Finished:
		m_sdk->GetWinnigPlayer();

		QMetaObject::invokeMethod(this, "ShowStartingWindow", Qt::QueuedConnection);
		return;
	default:
		return;
	}

	QMetaObject::invokeMethod(this, "ShowMessage", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(message)));
}

void GameScene::OnWindmill()
{
	QString message("You are now in windmill. Delete an oponent's piece.");
	QMetaObject::invokeMethod(this, "ShowMessage", Qt::QueuedConnection, Q_ARG(QString, message));

	m_sdk->GetPossibleRemoves();
}

void GameScene::OnPossibleMoves(PiecesIndexes indexes)
{
	try
	{
		m_board->HighlightNodes(indexes);
	}
	catch (std::exception e)
	{
		QMetaObject::invokeMethod(this, "ShowErrorMessage", Qt::QueuedConnection, Q_ARG(std::string, e.what()));
	}
}

void GameScene::OnPossibleRemoves(PiecesIndexes indexes)
{
	try
	{
		m_board->HighlightNodes(indexes);
	}
	catch (std::exception e)
	{
		QMetaObject::invokeMethod(this, "ShowErrorMessage", Qt::QueuedConnection, Q_ARG(std::string, e.what()));
	}
}

void GameScene::OnServerDisconnect()
{
	QMetaObject::invokeMethod(this, "ShowMessage", Qt::QueuedConnection
		, Q_ARG(QString, QString::fromStdString("lost connection to server, please restart")));
	QMetaObject::invokeMethod(this, "ShowStartingWindow", Qt::QueuedConnection);
}

void GameScene::SetBoard(IUiBoard* board)
{
	m_board = board;
}

void GameScene::SetLabels(EPieceType playerType, bool isComputer)
{
	auto label = "Moving player: " + EPieceTypeStringMaker::GetStringFromEnum(playerType);

	m_movingPlayerLabel->setText(QString::fromStdString(label));

	QString message;

	if (m_gameState == EGameState::Removing)
	{
		message = isComputer ? "Computer deleted a piece" : "You can now delete an opponent's piece. Right-click to do so.";
		QMetaObject::invokeMethod(this, "ShowMessage", Qt::QueuedConnection, Q_ARG(QString, QString::fromStdString(message.toStdString())));
	}
	else
	{
		if (isComputer)
		{
			QString playerType = m_movingPlayerLabel->text();
			playerType.append(" (computer) ");
			m_movingPlayerLabel->setText(playerType);
			m_computerThinking->setText(QString::fromStdString("Computer is thinking..."));
		}
		else {
			m_computerThinking->setText("");
		}
	}

	m_errorLabel->setText("");
}

void GameScene::OnSceneChange(IClientSDKPtr sdk, EBoardType type, EBoardSize size, NodesInfo nodeInfoList)
{
	m_sdk = sdk;
	SetBoard(IUiBoard::Produce(type, size, nodeInfoList));
	//InitGraphics();
}

void GameScene::UpdateBoard(NodesInfo nodeInfoList)
{
	m_board->UpdateBoard(nodeInfoList);
}

void GameScene::ShowMessage(const QString& message)
{
	auto messageBox = new QMessageBox(this);
	messageBox->setText(message);
	messageBox->setIcon(QMessageBox::Information);
	messageBox->setStyleSheet("background-color: rgb(255, 255, 255);");
	messageBox->show();
}

void GameScene::InitGraphics()
{
	setStyleSheet(windowBackground);

	if (!m_board)
	{
		ShowMessage("Board not set");
	}

	InitWidgets();

	InitLayouts();

	m_board->update();

	ConnectMethods();

}

void GameScene::showEvent(QShowEvent* event)
{
	m_sdk->SetListener(this);
	m_timer = new QTimer(this);
	m_timer->start(250);
	if (m_firstShow) {
		m_vertical1 = findChild<QVBoxLayout*>("vertical1");
		InitGraphics();
		connect(m_timer, &QTimer::timeout, this, &GameScene::UpdateTimer);
		m_firstShow = false;
	}
	update();
}

void GameScene::ConnectMethods()
{
	if (UiBoard* board = dynamic_cast<UiBoard*>(m_board))
	{
		connect(board, &UiBoard::nodeClicked, this, &GameScene::OnNodeClicked);
		connect(board, &UiBoard::removeNodeClicked, this, &GameScene::OnRemoveNodeClicked);
		connect(board, &UiBoard::move, this, &GameScene::OnMove);
		connect(board, &UiBoard::highlightMove, this, &GameScene::OnHighlightMove);
		connect(m_undoButton, &QPushButton::clicked, this, &GameScene::OnUndo);
	}
}

void GameScene::InitLayouts()
{
	QHBoxLayout* buttonsLayout = new QHBoxLayout();

	buttonsLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	buttonsLayout->addWidget(m_errorLabel);
	buttonsLayout->addWidget(m_undoButton, Qt::AlignHCenter);
	buttonsLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	QHBoxLayout* labelLayout = new QHBoxLayout();

	labelLayout->addWidget(m_movingPlayerLabel);
	labelLayout->addWidget(m_computerThinking);
	labelLayout->addWidget(m_timerLabel);

	m_vertical1->addLayout(labelLayout);
	m_vertical1->addWidget(m_board);
	m_vertical1->addLayout(buttonsLayout);
}

void GameScene::InitWidgets()
{
	CustomizeLabels(m_errorLabel);
	CustomizeLabels(m_movingPlayerLabel);
	CustomizeLabels(m_timerLabel);
	CustomizeLabels(m_computerThinking);

	m_board->setMinimumHeight(700);
	m_board->setMinimumWidth(700);

	m_errorLabel->setStyleSheet("QLabel { color : red; }");
	m_timerLabel->setStyleSheet("QLabel { color : red; }");

	m_sdk->GetActivePlayer();

	InitButtons();
}

void GameScene::CustomizeLabels(QLabel* label)
{
	QFont font;
	font.setPointSize(15);

	label->setAlignment(Qt::AlignHCenter);
	label->setFixedHeight(30);
	label->setFont(font);
}

void GameScene::InitButtons()
{
	m_undoButton->setStyleSheet(buttonStyle);
	m_undoButton->setText("Undo Move");
	m_undoButton->setFixedWidth(200);
	m_undoButton->setFixedHeight(50);
	m_undoButton->show();
}

void GameScene::ShowStartingWindow()
{
	//TO DO: switch back to menu
}

void GameScene::OnNodeClicked(uint8_t index)
{
	if (m_gameState == EGameState::Placing)
	{
		m_sdk->PlacePiece(index);
	}
}

void GameScene::OnRemoveNodeClicked(uint8_t index)
{
	m_sdk->RemovePiece(index);
}

void GameScene::OnMove(uint8_t fromIndex, uint8_t toIndex)
{
	m_sdk->MovePiece(fromIndex, toIndex);
}

void GameScene::OnHighlightMove(uint8_t index)
{
	try
	{
		m_sdk->GetPossibleMoves(index);
	}
	catch (std::exception e)
	{
		ShowErrorMessage(e.what());
	}
}

void GameScene::OnHighlightRemove()
{
	m_sdk->GetPossibleRemoves();
}

void GameScene::ShowErrorMessage(const std::string& error)
{
	m_errorLabel->setText(QString::fromStdString(error));

	QTimer::singleShot(1000, this, [this]() {
		m_errorLabel->clear();
		});
}

void GameScene::OnUndo()
{
	m_sdk->Undo();
}

void GameScene::UpdateTimer()
{
	m_sdk->GetTimer();
}

void GameScene::paintEvent(QPaintEvent* event)
{

}
