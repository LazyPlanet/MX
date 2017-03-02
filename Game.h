#pragma once

#include "P_Header.h"
#include "Player.h"
#include "Room.h"

#include <list>
#include <memory>
#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>

namespace Adoter
{

#define CARDS_COUNT 136

/////////////////////////////////////////////////////
//一场游戏
/////////////////////////////////////////////////////
class Game : public std::enable_shared_from_this<Game>
{
private:
	std::vector<int64_t> _hupai_players;

	std::list<int32_t> _cards; //随机牌,每次开局更新,索引为GameManager牌中索引

	size_t _banker_index; //庄家索引

	std::shared_ptr<Room> _room;
	
	std::unordered_map<int64_t, std::shared_ptr<Player>> _players; //房间中的玩家
public:
	virtual void Init(); //初始化
	virtual bool Start(std::unordered_map<int64_t, std::shared_ptr<Player>>& players); //开始游戏
	virtual void OnStart();
	virtual bool Over(); //游戏结束
	virtual std::vector<int32_t> FaPai(size_t card_count); //发牌
};

/////////////////////////////////////////////////////
//游戏通用管理类
/////////////////////////////////////////////////////
class GameManager
{
private:
	std::unordered_map<int32_t/*牌索引*/, Asset::Pai/*牌值*/> _cards;
public:
	static GameManager& Instance()
	{
		static GameManager _instance;
		return _instance;
	}

	bool Load(); //加载麻将牌数据

	Asset::Pai GetCard(int32_t card_index) 
	{
		auto it = _cards.find(card_index);
		if (it != _cards.end()) return it->second; 
		return {};
	}
};

#define GameInstance GameManager::Instance()

}
