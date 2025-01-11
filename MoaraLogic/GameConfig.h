#pragma once
#include "EComputerLevel.h"

#include "IGameConfig.h"

class GameConfig : public IGameConfig
{
public:
	GameConfig()
		: m_boardConfig()
		, m_playerConfig()
		, m_numberOfPiecesToPlace(9)
		, m_computerLevel(EComputerLevel::Easy)
	{

	}
	GameConfig(const BoardConfig& boardConfig, const PlayerConfig& playerConfig, int numberOfPiecesToPlace, EComputerLevel computerLevel)
		: m_boardConfig(boardConfig)
		, m_playerConfig(playerConfig)
		, m_numberOfPiecesToPlace(numberOfPiecesToPlace)
		, m_computerLevel(computerLevel)
	{

	}

	//IGameConfig
	void SetPlayerConfig(PlayerConfig& config) override;
	void SetBoardConfig(EBoardType type, EBoardSize size) override;
	void SetComputerLevel(EComputerLevel level) override;
	void SetNumberOfPlayers(int numberOfPlayers) override;
	void SetPlayersConfig(ConfigurationsMatrix matrix, int indexOfMatrix) override;

	BoardConfigMatrix GetBoardConfigMatrix() const override;
	EBoardType GetBoardType() const override;
	EBoardSize GetBoardSize() const override;

	Config GetPlayerConfig() const override;
	int GetNumberOfPiecesToPlace() const override;
	int GetNumberOfPlayers() const override;

	EComputerLevel GetComputerLevel() const override;

	std::string GetBoardMatrixName(EBoardSize size, EBoardType type) override;

	void SetPiecesToPlace(int number) override;
	void SetPlayersConfig(Config& config) override;

private:
	BoardConfig m_boardConfig;
	PlayerConfig m_playerConfig;
	EComputerLevel m_computerLevel;
	int m_numberOfPiecesToPlace;
};

inline void GameConfig::SetPlayersConfig(Config& config)
{
	m_playerConfig.SetPlayersConfig(config);
}

inline void GameConfig::SetPiecesToPlace(int number)
{
	m_numberOfPiecesToPlace = number;
}

inline std::string GameConfig::GetBoardMatrixName(EBoardSize size, EBoardType type)
{
	return m_boardConfig.GetBoardMatrixName(size, type);
}
inline void GameConfig::SetNumberOfPlayers(int numberOfPlayers)
{
	m_playerConfig.SetNumberOfPlayers(numberOfPlayers);
}

inline void GameConfig::SetPlayersConfig(ConfigurationsMatrix matrix, int indexOfMatrix)
{
	m_playerConfig.SetPlayersConfig(matrix[indexOfMatrix]);
}

inline BoardConfigMatrix GameConfig::GetBoardConfigMatrix() const
{
	return m_boardConfig.GetBoardConfig();
}

inline Config GameConfig::GetPlayerConfig() const
{
	return m_playerConfig.GetConfig();
}

inline int GameConfig::GetNumberOfPlayers() const
{
	return m_playerConfig.GetNumberOfPlayers();
}

inline EComputerLevel GameConfig::GetComputerLevel() const
{
	return m_computerLevel;
}

inline EBoardType GameConfig::GetBoardType() const
{
	return m_boardConfig.GetBoardType();
}

inline EBoardSize GameConfig::GetBoardSize() const
{
	return m_boardConfig.GetBoardSize();
}

inline int GameConfig::GetNumberOfPiecesToPlace() const
{
	return m_numberOfPiecesToPlace;
}

inline void GameConfig::SetPlayerConfig(PlayerConfig& config)
{
	m_playerConfig = config;
}

inline void GameConfig::SetComputerLevel(EComputerLevel level)
{
	m_computerLevel = level;
}

inline void GameConfig::SetBoardConfig(EBoardType type, EBoardSize size)
{
	BoardConfig newConfig(type, size);
	m_boardConfig = newConfig;
}