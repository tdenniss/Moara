#pragma once
#include "EPieceType.h"
#include "EComputerLevel.h"
#include "EBoardSize.h"
#include "EBoardType.h"
#include "EGameState.h"

#include <string>
#include <stdexcept>

class EPieceTypeStringMaker
{
public:
	static std::string GetStringFromEnum(EPieceType pieceType)
	{
		switch (pieceType)
		{
		case EPieceType::Black:
			return "Black";
			break;
		case EPieceType::White:
			return "White";
			break;
		case EPieceType::Red:
			return "Red";
			break;
		case EPieceType::None:
			return "None";
			break;
		default:
			return "";
			break;
		}
	}

	static EPieceType GetEnumFromString(const std::string& pieceTypeStr)
	{
		if (pieceTypeStr == "Black")
		{
			return EPieceType::Black;
		}
		else if (pieceTypeStr == "White")
		{
			return EPieceType::White;
		}
		else if (pieceTypeStr == "Red")
		{
			return EPieceType::Red;
		}
		else if (pieceTypeStr == "Yellow")
		{
			return EPieceType::Yellow;
		}
		else if (pieceTypeStr == "None")
		{
			return EPieceType::None;
		}
		else
		{
			throw std::invalid_argument("Invalid piece type string");
		}
	}
};

class EComputerLevelStringMaker
{
public:
	static std::string GetStringFromEnum(EComputerLevel level)
	{
		switch (level)
		{
		case EComputerLevel::None:
			return "None";
		case EComputerLevel::Easy:
			return "Easy";
		case EComputerLevel::Medium:
			return "Medium";
		case EComputerLevel::Hard:
			return "Hard";
		default:
			return "Unknown level";
		}
	}

	static EComputerLevel GetEnumFromString(const std::string& levelStr)
	{
		if (levelStr == "None") {
			return EComputerLevel::None;
		}
		else if (levelStr == "Easy") {
			return EComputerLevel::Easy;
		}
		else if (levelStr == "Medium") {
			return EComputerLevel::Medium;
		}
		else if (levelStr == "Hard") {
			return EComputerLevel::Hard;
		}
		else {
			throw std::invalid_argument("Invalid computer level string");
		}
	}
};

class EBoardTypeStringMaker
{
public:
	static std::string GetStringFromEnum(EBoardType boardType)
	{
		switch (boardType)
		{
		case EBoardType::Normal:
			return "Normal";
		case EBoardType::Diagonals:
			return "Diagonals";
		case EBoardType::None:
			return "None";
		default:
			return "Unknown";
		}
	}

	static EBoardType GetEnumFromString(const std::string& boardTypeStr)
	{
		if (boardTypeStr == "Normal") {
			return EBoardType::Normal;
		}
		else if (boardTypeStr == "Diagonals") {
			return EBoardType::Diagonals;
		}
		else if (boardTypeStr == "None") {
			return EBoardType::None;
		}
		else {
			throw std::invalid_argument("Invalid board type string: " + boardTypeStr);
		}
	}
};

class EBoardSizeStringMaker
{
public:
	static std::string GetStringFromEnum(EBoardSize boardSize)
	{
		switch (boardSize)
		{
		case EBoardSize::Normal:
			return "Normal";
		case EBoardSize::Big:
			return "Big";
		case EBoardSize::None:
			return "None";
		default:
			return "Unknown";
		}
	}

	static EBoardSize GetEnumFromString(const std::string& boardSizeStr)
	{
		if (boardSizeStr == "Normal") {
			return EBoardSize::Normal;
		}
		else if (boardSizeStr == "Big") {
			return EBoardSize::Big;
		}
		else if (boardSizeStr == "None") {
			return EBoardSize::None;
		}
		else {
			throw std::invalid_argument("Invalid board size string: " + boardSizeStr);
		}
	}
};

class EGameStateStringMaker
{
public:
	static std::string GetStringFromEnum(EGameState gameState)
	{
		switch (gameState)
		{
		case EGameState::Placing:
			return "Placing";
		case EGameState::Moving:
			return "Moving";
		case EGameState::Removing:
			return "Removing";
		case EGameState::Finished:
			return "Finished";
		default:
			return "Unknown";
		}
	}
	static EGameState GetEnumFromString(const std::string& gameStateStr)
	{
		if (gameStateStr == "Placing") {
			return EGameState::Placing;
		}
		else if (gameStateStr == "Moving") {
			return EGameState::Moving;
		}
		else if (gameStateStr == "Removing") {
			return EGameState::Removing;
		}
		else if (gameStateStr == "Finished") {
			return EGameState::Finished;
		}
		else {
			throw std::invalid_argument("Invalid game state string: " + gameStateStr);
		}
	}
};