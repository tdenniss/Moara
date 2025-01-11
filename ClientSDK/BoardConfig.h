#pragma once
#include "EBoardType.h"
#include "EBoardSize.h"

#include "BoardConfigurations.h"

#include <vector>
#include <string>

struct BoardConfig
{
public:
	BoardConfig(EBoardType type = EBoardType::Normal, EBoardSize size = EBoardSize::Normal);
	~BoardConfig() = default;

	BoardConfigMatrix GetBoardConfig() const;
	EBoardType GetBoardType() const;
	EBoardSize GetBoardSize() const;
	std::string GetBoardMatrixName(EBoardSize size, EBoardType type) const;
private:
	BoardConfigMatrix m_nodesMatrix;
	EBoardType m_type;
	EBoardSize m_size;
};

inline BoardConfig::BoardConfig(EBoardType type, EBoardSize size)
	: m_type(type)
	, m_size(size)
{
	if (type == EBoardType::Normal && size == EBoardSize::Normal)
		m_nodesMatrix = NORMAL_BOARD_CONFIG;
	else if (type == EBoardType::Normal && size == EBoardSize::Big)
		m_nodesMatrix = NORMAL_BIG_BOARD_CONFIG;
	else if (type == EBoardType::Diagonals && size == EBoardSize::Normal)
		m_nodesMatrix = DIAGONALS_NORMAL_BOARD_CONFIG;
	else if (type == EBoardType::Diagonals && size == EBoardSize::Big)
		m_nodesMatrix = DIAGONALS_BIG_BOARD_CONFIG;
}

inline BoardConfigMatrix BoardConfig::GetBoardConfig() const
{
	return m_nodesMatrix;
}

inline EBoardType BoardConfig::GetBoardType() const
{
	return m_type;
}

inline EBoardSize BoardConfig::GetBoardSize() const
{
	return m_size;
}

inline std::string BoardConfig::GetBoardMatrixName(EBoardSize size, EBoardType type) const
{
	if (type == EBoardType::Normal && size == EBoardSize::Normal)
		return "NORMAL_BOARD_CONFIG";
	if (type == EBoardType::Normal && size == EBoardSize::Big)
		return "NORMAL_BIG_BOARD_CONFIG";
	if (type == EBoardType::Diagonals && size == EBoardSize::Normal)
		return "DIAGONALS_NORMAL_BOARD_CONFIG";
	return "DIAGONALS_BIG_BOARD_CONFIG";
}