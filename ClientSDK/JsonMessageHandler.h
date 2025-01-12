#pragma once
#include "EnumStringMaker.h"
#include "GameConfig.h"

#include "json.hpp"

using Json = nlohmann::json;

using NodesInfo = std::vector<std::pair<int, EPieceType>>;

class JsonMessageHandler
{
public:
	template<typename ... Args>
	static std::pair<void*, int> MakeJson(const void* command, Args ... args)
	{
		Json json;
		json["action"] = (char*)command;

		if constexpr (sizeof...(args) > 0) {
			Json parameters;

			auto add_to_parameters = [&parameters](const auto& pair) {
				parameters[pair.first] = pair.second;
				};

			(add_to_parameters(args), ...);

			json["parameters"] = parameters;
		}
		else
			json["parameters"] = "";

		std::string jsonString = json.dump();
		char* buffer = new char[jsonString.size() + 1];
		std::memcpy(buffer, jsonString.c_str(), jsonString.size() + 1);

		return std::make_pair(static_cast<void*>(buffer), static_cast<int>(jsonString.size()));
	}

	template<typename T>
	static T GetFromParam(const void* tag, const void* data)
	{
		Json json = Json::parse((char*)data);

		Json param = json["parameters"];

		return param[(char*)tag].get<T>();
	}

	template<typename T>
	static T GetFromCommand(const void* tag, const void* data)
	{
		Json json = Json::parse((char*)data);

		return json[(char*)tag].get<T>();
	}

	static GameConfigPtr BuildGameConfig(const void* data)
	{
		Json jsonData = Json::parse((char*)data);

		Json param = jsonData["parameters"];

		GameConfigPtr config = std::make_shared<GameConfig>();

		config->SetPiecesToPlace(param["numberOfPieces"].get<int>());

		auto level = EComputerLevelStringMaker::GetEnumFromString(param["computerLevel"].get<std::string>());

		config->SetComputerLevel(level);

		config->SetNumberOfPlayers(param["numberOfPlayers"].get<int>());

		Config pConfig;

		for (auto it = param.begin(); it != param.end(); ++it)
		{
			if (it.key().find("player_") != std::string::npos)
			{
				const Json& playerDetailsJson = it.value();

				int playerType = playerDetailsJson["action"].get<int>();
				bool isAI = playerDetailsJson["ai"].get<bool>();

				pConfig.emplace_back(playerType, isAI);
			}
		}

		config->SetPlayersConfig(pConfig);

		auto boardType = EBoardTypeStringMaker::GetEnumFromString(param["boardType"].get<std::string>());
		auto boardSize = EBoardSizeStringMaker::GetEnumFromString(param["boardSize"].get<std::string>());

		config->SetBoardConfig(boardType, boardSize);

		return config;
	}

	static void BuildPlayersJson(Config& config, Json& configJson)
	{
		int count = 1;

		for (auto& player : config)
		{
			Json playerJson =
			{
				{"action", player.first},
				{"ai", player.second}
			};

			configJson["player_" + std::to_string(count)] = playerJson;

			count++;
		}
	}

	static std::pair<void*, int> MakeGameConfigJson(GameConfigPtr config)
	{
		Json parameters;

		parameters["numberOfPieces"] = config->GetNumberOfPiecesToPlace();

		parameters["computerLevel"] = EComputerLevelStringMaker::GetStringFromEnum(config->GetComputerLevel());

		parameters["numberOfPlayers"] = config->GetNumberOfPlayers();

		auto playersConfig = config->GetPlayerConfig();
		BuildPlayersJson(playersConfig, parameters);

		parameters["boardType"] = EBoardTypeStringMaker::GetStringFromEnum(config->GetBoardType());
		parameters["boardSize"] = EBoardSizeStringMaker::GetStringFromEnum(config->GetBoardSize());

		Json message;

		message["action"] = "startGame";
		message["parameters"] = parameters;

		std::string jsonString = message.dump();
		char* buffer = new char[jsonString.size() + 1];
		std::memcpy(buffer, jsonString.c_str(), jsonString.size() + 1);

		return std::make_pair(static_cast<void*>(buffer), static_cast<int>(jsonString.size()));
	}

	static NodesInfo GetNodesFromJson(const void* data)
	{
		Json json = Json::parse((char*)data)["parameters"];

		NodesInfo nodeInfoList;

		for (const auto& nodeJson : json["nodes"])
		{
			int index = nodeJson.at("index").get<int>();
			std::string typeStr = nodeJson.at("type").get<std::string>();
			EPieceType type = EPieceTypeStringMaker::GetEnumFromString(typeStr);

			nodeInfoList.emplace_back(index, type);
		}

		return nodeInfoList;
	}
};