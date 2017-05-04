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
	
extern const int32_t MAX_PLAYER_COUNT; //玩家总数：有些地方不是4人麻将

/////////////////////////////////////////////////////
//一场游戏
/////////////////////////////////////////////////////
class Game : public std::enable_shared_from_this<Game>
{
	std::shared_ptr<Room> _room = nullptr; //游戏在哪个房间开启

private:
	
	std::list<int32_t> _cards; //随机牌,每次开局更新,索引为GameManager牌中索引
	std::vector<int64_t> _hupai_players;

	int32_t _curr_player_index = 0; //当前在操作的玩家索引
	int64_t _banker_player_id = 0; //庄家
	std::vector<int64_t> _ting_players; //听牌玩家

	Asset::PaiOperationLimit _oper_limit; //牌操作限制
	Asset::PaiElement _baopai; //宝牌
	
	std::vector<Asset::PaiOperationList> _oper_list; //可操作列表

	std::shared_ptr<Player> _players[MAX_PLAYER_COUNT]; //玩家数据：按照进房间的顺序，0->1->2->3...主要用于控制发牌和出牌顺序
public:
	virtual void Init(std::shared_ptr<Room> room); //初始化
	virtual bool Start(std::vector<std::shared_ptr<Player>> players); //开始游戏
	virtual void OnStart(); //开始游戏回调
	virtual bool OnOver(); //游戏结束

	virtual std::vector<int32_t> FaPai(size_t card_count); //发牌
	virtual std::vector<int32_t> TailPai(size_t card_count); //后楼发牌
	
	void OnPaiOperate(std::shared_ptr<Player> player, pb::Message* message);
	bool CanPaiOperate(std::shared_ptr<Player> player, pb::Message* message);
	void OnOperateTimeOut();
	void ClearOperation();
	bool SendCheckRtn();
	bool CheckPai(const Asset::PaiElement& pai, int64_t from_player_id); //检查牌形：返回待操作的玩家ID
	
	//获取下家
	std::shared_ptr<Player> GetNextPlayer(int64_t player_id);
	//获取玩家
	std::shared_ptr<Player> GetPlayer(int64_t player_id);
	std::shared_ptr<Player> GetPlayerByOrder(int32_t player_index);
	//获取玩家的顺序
	int32_t GetPlayerOrder(int32_t player_id);
	//设置房间
	void SetRoom(std::shared_ptr<Room> room) {	_room = room; }
	//是否庄家
	bool IsBanker(int64_t player_id);

	void BroadCast(pb::Message* message, int64_t exclude_player_id = 0);
	void BroadCast(pb::Message& message, int64_t exclude_player_id = 0);

	void Calculate(int64_t hupai_player_id/*胡牌玩家*/, int64_t dianpao_player_id/*胡牌玩家*/, std::vector<Asset::FAN_TYPE>& fan_list/*基础分*/);
	//增加听牌玩家
	void AddTingPlayer(int64_t player_id) {	_ting_players.push_back(player_id);	}
	//设置宝牌
	void SetBaoPai(const Asset::PaiElement& pai) { _baopai = pai; }
	//获取宝牌
	const Asset::PaiElement& GetBaoPai() { return _baopai; }
	Asset::PaiElement GetBaopai(int32_t tail_index);
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
