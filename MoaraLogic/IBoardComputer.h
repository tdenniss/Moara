#pragma once
#include "EPieceType.h"

#include <vector>
#include <memory>

using NodeList = std::vector<class INode*>;
using PieceIndexes = std::vector<uint8_t>;
using MovingPlayerPtr = std::shared_ptr<class IPlayer>;
using EPlayerType = EPieceType;

class IBoardComputer
{
public:
    virtual NodeList GetAllNodes() const = 0;
    virtual PieceIndexes GetPossiblePlaces() const = 0;
    virtual PieceIndexes GetPossibleMovesFromNode(uint8_t nodeIndex, EPlayerType player) const = 0;
    virtual PieceIndexes GetPossibleMoves(EPlayerType player) const = 0;
    virtual PieceIndexes GetPossibleRemoves(EPlayerType player) const = 0;

    virtual bool CanBeFullLine(uint8_t nodeIndex, EPieceType nodePieceType = EPieceType::None, uint8_t indexToIgnore = 255) const = 0;
    virtual bool IsFullLine(uint8_t nodeIndex, EPieceType currentNodeType = EPieceType::None, uint8_t indexToIgnore = 255, bool windmill = false) const = 0;
};
