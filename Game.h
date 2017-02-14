#pragma once

#include "P_Header.h"
#include "Player.h"

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
	std::vector<std::shared_ptr<Player>> _players; //本次游戏参与的玩家
	static int32_t _banker_index; //庄家索引
public:
	virtual void Init(); //初始化
	virtual bool Start(); //开始游戏
	virtual bool Over(); //游戏结束
	virtual void FaPai(uint32_t card_count, std::vector<int32_t>& cards); //发牌
};

/////////////////////////////////////////////////////
//游戏通用管理类
/////////////////////////////////////////////////////
class GameManager
{
private:
	std::unordered_map<int32_t, Asset::MJCard_Card*> _cards;
public:
	static GameManager& Instance()
	{
		static GameManager _instance;
		return _instance;
	}

	bool Load(); //加载麻将牌数据

	const Asset::MJCard_Card* GetCard(uint32_t index) {
		if (index <= 0 || index > _cards.size()) return nullptr;
		return _cards[index]; 
	}
};

#define GameInstance GameManager::Instance()

}
