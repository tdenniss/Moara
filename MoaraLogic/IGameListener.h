#pragma once

#include "EPieceType.h"
#include "EGameState.h"

#include <stdint.h>
#include <memory>

using GameListenerPtr = std::shared_ptr<class IGameListener>;
using ListenerWeakPtr = std::weak_ptr<class IGameListener>;

class IGameListener
{
public:
    virtual void OnMoveMade(uint8_t fromNodeIndex, uint8_t toNodeIndex, EPieceType fromNodeType) = 0;
    virtual void OnAddedPiece(uint8_t addedNodeIndex, EPieceType nodeType) = 0;
    virtual void OnRemovedPiece(uint8_t removedNodeIndex) = 0;
    virtual void OnGameStateChanged(EGameState newState) = 0;
    virtual void OnWindmillRule() = 0;
    virtual void OnPlayerChanged(EPieceType playerType) = 0;
    virtual void OnPlayerRemoved(EPieceType who) = 0;
};
