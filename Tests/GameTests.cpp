#include "pch.h"

#include "Game.h"
#include "GameConfig.h"

#include "EasyComputerLevel.h"
#include "MediumComputerLevel.h"
#include "HardComputerLevel.h"

#include "NormalBoard.h"

class ListenerDummy : public IGameListener
{

public:
	void OnMoveMade(uint8_t fromNodeIndex, uint8_t toNodeIndex, EPieceType fromNodeType) override
	{
		return;
	}


	void OnAddedPiece(uint8_t addedNodeIndex, EPieceType nodeType) override
	{
		return;
	}


	void OnRemovedPiece(uint8_t removedNodeIndex) override
	{
		return;
	}


	void OnGameStateChanged(EGameState newState) override
	{
		return;
	}


	void OnWindmillRule() override
	{
		return;
	}


	void OnPlayerChanged(EPieceType playerType, bool isComputer) override
	{
		return;
	}


	void OnPlayerRemoved(EPieceType who) override
	{
		return;
	}

};

class MockBoard : public IBoard
{
public:
	MOCK_METHOD(EPieceType, GetNodeType, (uint8_t nodeIndex), (const override));
	MOCK_METHOD(PieceTypeList, GetAllNodesType, (), (const override));
	MOCK_METHOD(NodeList, GetAllNodes, (), (const override));
	MOCK_METHOD(uint8_t, GetNodesCount, (), (const override));
	MOCK_METHOD(PieceIndexes, GetSamePieceTypeNodeIndexes, (EPieceType type), (const override));
	MOCK_METHOD(EBoardState, GetBoardState, (), (const override));
	MOCK_METHOD(uint8_t, GetPlayerPiecesOnTable, (EPlayerType player), (const override));
	MOCK_METHOD(uint8_t, GetPlayerPiecesToPlace, (EPlayerType player), (const override));
	MOCK_METHOD(PieceIndexes, GetPossiblePlaces, (), (const override));
	MOCK_METHOD(PieceIndexes, GetPossibleMovesFromNode, (uint8_t nodeIndex, EPlayerType player), (const override));
	MOCK_METHOD(PieceIndexes, GetPossibleMoves, (EPlayerType player), (const override));
	MOCK_METHOD(PieceIndexes, GetPossibleRemoves, (EPlayerType player), (const override));
	MOCK_METHOD(void, AddPiece, (uint8_t nodeIndex, EPlayerType player), (override));
	MOCK_METHOD(void, MovePiece, (uint8_t firstNodeIndex, uint8_t secondNodeIndex, EPlayerType movingPlayer), (override));
	MOCK_METHOD(void, RemovePiece, (uint8_t nodeIndex, EPlayerType player), (override));
	MOCK_METHOD(void, UndoAddPiece, (uint8_t nodeIndex), (override));
	MOCK_METHOD(void, UndoMovePiece, (uint8_t firstNodeIndex, uint8_t secondNodeIndex), (override));
	MOCK_METHOD(void, UndoRemovePiece, (uint8_t nodeIndex, EPlayerType player), (override));
	MOCK_METHOD(void, RemovePlayerPieces, (EPlayerType player), (override));
	MOCK_METHOD(bool, CanBeFullLine, (uint8_t nodeIndex, EPieceType nodePieceType, uint8_t indexToIgnore), (const override));
	MOCK_METHOD(bool, IsFullLine, (uint8_t nodeIndex, EPieceType currentNodeType, uint8_t indexToIgnore, bool windmill), (const override));
	MOCK_METHOD(void, IsMoveValid, (uint8_t firstNodeIndex, uint8_t secondNodeIndex, EPlayerType movingPlayer), (const override));
	MOCK_METHOD(bool, IsWindmillRule, (uint8_t nodeIndex, EPieceType currentNodeType), (const override));
	MOCK_METHOD(void, SetPlayerPiecesOnTable, (EPlayerType player, int piecesOnTable), (override));
	MOCK_METHOD(void, SetPlayerPiecesToPlace, (EPlayerType player, int piecesToPlace), (override));
	MOCK_METHOD(EBoardType, GetBoardType, (), (const override));
	MOCK_METHOD(void, LoadBoard, (const PieceTypeList& players, std::ifstream& file), (override));
	MOCK_METHOD(void, SaveBoard, (std::ofstream& file), (const override));
	MOCK_METHOD(void, LoadFromConfig, (const PieceTypeList& players, const BoardConfigMatrix& boardMatrix, int piecesToPlace), (override));
};

class ComputerMock : public IComputerLevel
{
public:
	MOCK_METHOD(uint8_t, GetPlacingIndex, (const BoardComputerPtr& board, EPieceType computerPieceType), (override));
	MOCK_METHOD(MovePosition, GetMovingIndex, (const BoardComputerPtr& board, EPieceType computerPieceType), (override));
	MOCK_METHOD(uint8_t, GetRemovingIndex, (const BoardComputerPtr& board, EPieceType computerPieceType), (override));
};

using::testing::Return;

class GameTest : public testing::Test
{

public:
	void RemoveTest(ComputerLevelPtr& computer)
	{
		PieceIndexes possibleRemoves{ 1 };

		EXPECT_CALL(*m_board, GetPossibleRemoves)
			.Times(1)
			.WillOnce(Return(possibleRemoves));

		EXPECT_EQ(computer->GetRemovingIndex(m_board, EPieceType::White), 1);
	}

	IBoardPtr LoadBoardFromFile(const std::string& filePath)
	{
		PieceTypeList players = { EPieceType::Black, EPieceType::White };

		std::ifstream file(filePath, std::ios_base::in);

		return std::make_shared<NormalBoard>(players, file);
	}

	void PlacingTest(ComputerLevelPtr& computer, const std::string& filePath, int indexToExpect, EPieceType pieceType)
	{
		auto board = LoadBoardFromFile(filePath);

		EXPECT_EQ(computer->GetPlacingIndex(board, pieceType), indexToExpect);
	}

	void MovingTest(ComputerLevelPtr& computer, const std::string& filePath, MovePosition posToExpect, EPieceType pieceType)
	{
		auto board = LoadBoardFromFile(filePath);

		EXPECT_EQ(computer->GetMovingIndex(board, pieceType), posToExpect);
	}

	template<class Game>
	void TestGameInit(Game game)
	{
		EXPECT_EQ(game->GetActivePlayer(), EPieceType::Black);
		EXPECT_EQ(game->GetGameState(), EGameState::Placing);

		EXPECT_EQ(game->GetWinningPlayer(), EPieceType::None);

		EXPECT_EQ(game->GetAllPlayerTypes().size(), 2);

		EXPECT_EQ(game->GetPlayerPiecesToPlace(EPlayerType::Black), 9);

		EXPECT_EQ(game->GetPlayerPiecesOnBoard(EPlayerType::Black), 0);
	}

protected:
	void SetUp()
	{
		m_computer = std::make_shared<ComputerMock>();

		m_board = std::make_shared<MockBoard>();
		m_unmockedBoard = std::make_shared<NormalBoard>();

		m_game = std::make_shared<Game>();
		m_nonmockedBoardGame = std::make_shared<Game>();

		m_easyComputer = std::make_shared<EasyComputerLevel>();
		m_mediumComputer = std::make_shared<MediumComputerLevel>(1.0);
		m_hardComputer = std::make_shared<HardComputerLevel>();

		GameConfigPtr allAiConfig = std::make_shared<GameConfig>();
		allAiConfig->SetPlayersConfig(smallAIBoardConfiguration, 1);

		m_oneAiConfig = std::make_shared<GameConfig>();
		m_oneAiConfig->SetPlayersConfig(smallAIBoardConfiguration, 0);

		m_nonmockedBoardGame->Initialize(allAiConfig);
		m_game->Initialize(m_oneAiConfig);

		m_game->SetComputerLevel(m_computer);
		m_nonmockedBoardGame->SetComputerLevel(m_computer);

		m_game->SetBoard(m_board);

		m_timer = std::make_shared<Timer>(Miliseconds(5000000));

		m_nonmockedBoardGame->SetTimer(m_timer);

	}

	std::shared_ptr<ComputerMock> m_computer;
	std::shared_ptr<MockBoard> m_board;
	IBoardPtr m_unmockedBoard;

	std::shared_ptr<Game> m_game;
	std::shared_ptr<Game> m_nonmockedBoardGame;

	ComputerLevelPtr m_easyComputer;
	ComputerLevelPtr m_mediumComputer;
	ComputerLevelPtr m_hardComputer;

	GameConfigPtr m_oneAiConfig;

	TimerPtr m_timer;
};

using ::testing::Return;

TEST_F(GameTest, GameProducing)
{
	GameConfigPtr config = std::make_shared<GameConfig>();

	auto game = IGame::Produce(config);

	TestGameInit(game);
}

TEST_F(GameTest, GameInitialization)
{
	m_game->SetBoard(m_unmockedBoard);

	TestGameInit(m_game);
}

struct GameStatus
{
	GameStatus(int player1Pieces, int player2Pieces, EPlayerType activePlayer, EGameState gameState)
		: player1Pieces{ player1Pieces }
		, player2Pieces{ player2Pieces }
		, activePlayer{ activePlayer }
		, gameState{ gameState }
	{
	}

	int player1Pieces;
	int player2Pieces;
	EPlayerType activePlayer;
	EGameState gameState;
};

TEST_F(GameTest, PlacePiece)
{
	EXPECT_CALL(*m_board, AddPiece)
		.Times(1)
		.WillOnce(Return());
	EXPECT_CALL(*m_board, GetPlayerPiecesToPlace)
		.Times(6)
		.WillRepeatedly(Return(8));
	EXPECT_CALL(*m_board, GetBoardState)
		.Times(1)
		.WillOnce(Return(EBoardState::None));

	m_game->PlacePiece(1);

	EXPECT_EQ(m_game->GetGameState(), EGameState::Placing);
	EXPECT_EQ(m_game->GetActivePlayer(), EPlayerType::White);
}

TEST_F(GameTest, EasyComputerPlacing)
{
	std::vector<INode*> possiblePlacingNodes
	{
		new NormalNode(),
		new NormalNode(),
		new NormalNode(),
		new NormalNode
	};

	EXPECT_CALL(*m_board, GetAllNodes)
		.Times(1)
		.WillOnce(Return(possiblePlacingNodes));

	bool foundIndex = false;
	auto randomIndex = m_easyComputer->GetPlacingIndex(m_board, EPieceType::White);

	for (auto& node : possiblePlacingNodes)
	{
		if (node->GetIndex() == randomIndex)
		{
			foundIndex = true;
			break;
		}
	}

	EXPECT_EQ(foundIndex, true);
}

TEST_F(GameTest, EasyComputerMoving)
{
	PieceIndexes possiblePlacingIndexes{ 1 };

	NodeList nodes
	{
		new NormalNode
	};

	nodes[0]->SetPiece(EPieceType::White);

	EXPECT_CALL(*m_board, GetAllNodes)
		.Times(1)
		.WillOnce(Return(nodes));

	EXPECT_CALL(*m_board, GetPossibleMoves)
		.Times(1)
		.WillOnce(Return(possiblePlacingIndexes));

	EXPECT_CALL(*m_board, GetPossibleMovesFromNode)
		.Times(1)
		.WillOnce(Return(possiblePlacingIndexes));

	MovePosition pos(0, 1);
	EXPECT_EQ(m_easyComputer->GetMovingIndex(m_board, EPieceType::White), pos);
}

TEST_F(GameTest, LetComputerPlace)
{
	EXPECT_CALL(*m_computer, GetPlacingIndex)
		.Times(1)
		.WillOnce(Return(1));

	m_nonmockedBoardGame->LetComputerPlay();

	EXPECT_NE(m_nonmockedBoardGame->GetNode(1), EPieceType::None);
}

TEST_F(GameTest, LetComputerMove)
{
	EXPECT_CALL(*m_computer, GetMovingIndex)
		.Times(1)
		.WillOnce(Return(MovePosition(0, 1)));

	m_nonmockedBoardGame->PlacePiece(0);
	m_nonmockedBoardGame->PlacePiece(2);

	m_nonmockedBoardGame->SetGameState(EGameState::Moving);
	m_nonmockedBoardGame->LetComputerPlay();

	EXPECT_NE(m_nonmockedBoardGame->GetNode(1), EPieceType::None);
	EXPECT_EQ(m_nonmockedBoardGame->GetNode(0), EPieceType::None);
}

TEST_F(GameTest, LetComputerRemove)
{
	EXPECT_CALL(*m_computer, GetRemovingIndex)
		.Times(1)
		.WillOnce(Return(1));

	m_nonmockedBoardGame->PlacePiece(1);

	EXPECT_TRUE(m_nonmockedBoardGame->IsComputerTurn());
	EXPECT_EQ(m_nonmockedBoardGame->GetPiece(1), EPieceType::Black);

	m_nonmockedBoardGame->SetGameState(EGameState::Removing);
	m_nonmockedBoardGame->LetComputerPlay();

	EXPECT_EQ(m_nonmockedBoardGame->GetNode(1), EPieceType::None);
}

TEST_F(GameTest, PossibleMoves)
{
	m_nonmockedBoardGame->PlacePiece(0);
	m_nonmockedBoardGame->PlacePiece(1);
	m_nonmockedBoardGame->PlacePiece(2);
	m_nonmockedBoardGame->PlacePiece(3);
	m_nonmockedBoardGame->PlacePiece(4);
	m_nonmockedBoardGame->PlacePiece(5);
	m_nonmockedBoardGame->PlacePiece(6);
	m_nonmockedBoardGame->PlacePiece(20);

	PieceIndexes indexes{ 7 };
	PieceIndexes removes{ 1, 3, 5, 20 };

	EXPECT_EQ(m_nonmockedBoardGame->GetPossibleMovesFromNode(0), indexes);
	EXPECT_EQ(m_nonmockedBoardGame->GetPossibleRemoves(), removes);
}