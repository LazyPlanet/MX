#pragma once

#include <list>
#include <memory>
#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>

#include "P_Header.h"
#include "Player.h"
#include "Room.h"

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

	std::shared_ptr<Room> _room = nullptr; //游戏在哪个房间开启
	std::shared_ptr<Player> _player = nullptr; //当前可操作的玩家

	int32_t _banker_index = 0; //庄家索引
	int32_t _operation_index = 0; //操作索引

	
	std::unordered_map<int64_t, std::shared_ptr<Player>> _players; //房间中的玩家

	//boost::asio::deadline_timer _timer;
public:
	//Game(){	}
	virtual void Init(); //初始化
	virtual bool Start(std::unordered_map<int64_t, std::shared_ptr<Player>> players); //开始游戏
	virtual void OnStart(); //开始游戏回调
	virtual bool Over(); //游戏结束
	virtual std::vector<int32_t> FaPai(size_t card_count); //发牌
	
	void OnPaiOperate(std::shared_ptr<Player> player, pb::Message* message);
	bool CanPaiOperate(std::shared_ptr<Player> player, pb::Message* message);
	void OnOperateTimeOut();
	void ClearOperation();
	int32_t CheckPai(const Asset::PaiElement& pai, int64_t from_player_id = 0); //检查牌形
};

/////////////////////////////////////////////////////
//游戏通用管理类
/////////////////////////////////////////////////////
class GameManager
{
private:
	std::unordered_map<int32_t/*牌索引*/, Asset::PaiElement/*牌值*/> _cards;
	std::vector<shared_ptr<Game>> _games;
public:
	static GameManager& Instance()
	{
		static GameManager _instance;
		return _instance;
	}

	bool Load(); //加载麻将牌数据

	Asset::PaiElement GetCard(int32_t card_index) 
	{
		auto it = _cards.find(card_index);
		if (it != _cards.end()) return it->second; 
		return {};
	}
	
	void OnCreateGame(std::shared_ptr<Game> game);
};

#define GameInstance GameManager::Instance()

}
