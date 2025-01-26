#include "pch.h"

#include "Game.h"
#include "GameConfig.h"

#include "NormalBoard.h"

using ::testing::Return;

class MockListener : public IGameListener
{
public:
	MOCK_METHOD(void, OnMoveMade, (uint8_t fromNodeIndex, uint8_t toNodeIndex, EPieceType fromNodeType), (override));
	MOCK_METHOD(void, OnAddedPiece, (uint8_t addedNodeIndex, EPieceType nodeType), (override));
	MOCK_METHOD(void, OnRemovedPiece, (uint8_t removedNodeIndex), (override));
	MOCK_METHOD(void, OnGameStateChanged, (EGameState newState), (override));
	MOCK_METHOD(void, OnWindmillRule, (), (override));
	MOCK_METHOD(void, OnPlayerChanged, (EPieceType playerType, bool isComputer), (override));
	MOCK_METHOD(void, OnPlayerRemoved, (EPieceType who), (override));
};


class APITest : public testing::Test
{

public:

protected:
	void SetUp()
	{
		GameConfigPtr config = std::make_shared<GameConfig>();
		config->SetBoardConfig(EBoardType::Normal, EBoardSize::Normal);
		m_game = IGame::Produce(config);
		m_listener = std::make_shared<MockListener>();
		m_game->AddListener(m_listener);
	}

	std::shared_ptr<IGame> m_game;
	std::shared_ptr<MockListener> m_listener;
	
};

TEST_F(APITest, PlacePiece)
{
	EXPECT_CALL(*m_listener, OnAddedPiece)
		.Times(1)
		.WillOnce(Return());

	m_game->PlacePiece(1);
}

TEST_F(APITest, MovePiece)
{
	EXPECT_CALL(*m_listener, OnMoveMade)
		.Times(1)
		.WillOnce(Return());
	for (int i = 0; i < 17; i++)
	{
		m_game->PlacePiece(i);
	}
	m_game->PlacePiece(23);
	m_game->MovePiece(17, 18);
}