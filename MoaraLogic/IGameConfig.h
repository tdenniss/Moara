#pragma once
#include "PlayerConfig.h"
#include "BoardConfig.h"
#include "EComputerLevel.h"

#include <memory>

using GameConfigPtr = std::shared_ptr<class IGameConfig>;

class IGameConfig
{
public:
    virtual void SetPlayerConfig(PlayerConfig& config) = 0;
    virtual void SetBoardConfig(EBoardType type, EBoardSize size) = 0;
    virtual void SetComputerLevel(EComputerLevel level) = 0;
    virtual void SetNumberOfPlayers(int numberOfPlayers) = 0;
    virtual void SetPlayersConfig(ConfigurationsMatrix matrix, int indexOfMatrix) = 0;
    virtual BoardConfigMatrix GetBoardConfigMatrix() const = 0;
    virtual EBoardType GetBoardType() const = 0;
    virtual EBoardSize GetBoardSize() const = 0;
    virtual Config GetPlayerConfig() const = 0;
    virtual int GetNumberOfPiecesToPlace() const = 0;
    virtual int GetNumberOfPlayers() const = 0;
    virtual EComputerLevel GetComputerLevel() const = 0;
    virtual std::string GetBoardMatrixName(EBoardSize size, EBoardType type) = 0;

    virtual void SetPiecesToPlace(int number) = 0;

    virtual void SetPlayersConfig(Config& config) = 0;
};
