#pragma once
#include <vector>

using Config = std::vector<std::pair<int, bool>>;
using ConfigurationsMatrix = std::vector<Config>;

const ConfigurationsMatrix smallAIBoardConfiguration = {
			{std::make_pair(0, 0), std::make_pair(1, 1)},
			{std::make_pair(0, 1), std::make_pair(1, 1)}
};

const ConfigurationsMatrix bigAIBoardConfiguration = {
			{std::make_pair(0, 0), std::make_pair(1, 0), std::make_pair(2, 1)},
			{std::make_pair(0, 0), std::make_pair(1, 1), std::make_pair(2, 1)},
			{std::make_pair(0, 1), std::make_pair(1, 1), std::make_pair(2, 1)}
};
const ConfigurationsMatrix humanBoardConfiguration = {
			{std::make_pair(0, 0), std::make_pair(1, 0)},
			{std::make_pair(0, 0), std::make_pair(1, 0), std::make_pair(2, 0)},
};

struct PlayerConfig
{
public:
	PlayerConfig()
		: m_numberOfPlayers(2)
		, m_playersConfig(smallAIBoardConfiguration[0])
	{
	}
	PlayerConfig(int numberOfPlayers)
		: m_numberOfPlayers(numberOfPlayers)
	{
	}

	template<typename... Pairs>
	void SetPlayersConfig(Pairs... pairs);
	void SetPlayersConfig(Config& config);

	Config GetConfig() const;
	int GetNumberOfPlayers() const;
	void SetNumberOfPlayers(int numberOfPlayers);
private:
	Config m_playersConfig;
	int m_numberOfPlayers;
};

inline void PlayerConfig::SetPlayersConfig(Config& config)
{
	m_playersConfig = config;
}

template<typename ...Pairs>
inline void PlayerConfig::SetPlayersConfig(Pairs ...pairs)
{
	m_playersConfig = { pairs ... };
}

inline Config PlayerConfig::GetConfig() const
{
	return m_playersConfig;
}

inline int PlayerConfig::GetNumberOfPlayers() const
{
	return m_numberOfPlayers;
}

inline void PlayerConfig::SetNumberOfPlayers(int numberOfPlayers)
{
	m_numberOfPlayers = numberOfPlayers;
}
