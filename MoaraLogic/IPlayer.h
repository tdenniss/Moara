#pragma once
#include "EPieceType.h"

#include "Constants.h"

#include <memory>

using PlayerPtr = std::shared_ptr<class IPlayer>;
using EPlayerType = EPieceType;

class IPlayer
{
public:
	virtual EPlayerType GetType() const = 0;
	virtual void SetType(EPlayerType type) = 0;

	virtual bool IsComputer() const = 0;
	virtual void SetComputer(bool isComputer) = 0;

	virtual ~IPlayer() = default;
};