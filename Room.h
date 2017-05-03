#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>

#include "Asset.h"
#include "Player.h"

namespace Adoter
{

class Player;
class Game;

const int32_t MAX_PLAYER_COUNT = 4;

class Room : public std::enable_shared_from_this<Room>
{
	int32_t _banker_index = 0; //庄家索引
	int64_t _banker = 0; //庄家：玩家ID

	std::mutex _mutex;

private:
	std::shared_ptr<Asset::Room> _stuff; //数据
	std::vector<std::shared_ptr<Game>> _games;
	std::vector<std::shared_ptr<Player>> _players; //房间中的玩家：按照进房间的顺序，东南西北
public:
	explicit Room(Asset::Room room) {  _stuff = std::make_shared<Asset::Room>(room); }

	virtual int64_t GetID() { return _stuff->room_id(); }

	virtual void SetID(int64_t room_id) { return _stuff->set_room_id(room_id); }
	
	virtual std::shared_ptr<Asset::Room> Get() {
		return _stuff; //数据
	}
	
	const Asset::RoomOptions& GetOptions() {
		return _stuff->options(); //数据
	}

public:
	Asset::ERROR_CODE TryEnter(std::shared_ptr<Player> player);
	void Enter(std::shared_ptr<Player> player);

	void OnPlayerLeave(int64_t player_id);

	void OnCreated(); 

	bool IsFull() { return _players.size() >= (size_t)MAX_PLAYER_COUNT; } //房间是否已满

	bool CanStarGame(); //能否开启游戏

	void OnPlayerOperate(std::shared_ptr<Player> player, pb::Message* message);

	void BroadCast(pb::Message* message, int64_t exclude_player_id = 0);
	void BroadCast(pb::Message& message, int64_t exclude_player_id = 0);
	
	void SyncRoom(); //房间数据

	//获取房主
	std::shared_ptr<Player> GetHoster();
	//是否是房主
	bool IsHoster(int64_t player_id);
	//获取房间中的玩家
	std::shared_ptr<Player> GetPlayer(int64_t player_id);
	//删除玩家
	bool Remove(int64_t player_id);
	//游戏结束
	void GameOver(int64_t player_id/*胡牌玩家*/);
	
	//庄家信息
	void SetBanker(int64_t player_id) { _banker = player_id; } //设置庄家
	int64_t GetBanker() { return _banker; } //获取庄家
	int32_t GetBankerIndex() { return _banker_index; } //庄家索引
	bool IsBanker(int64_t player_id){ return _banker == player_id; } //是否是庄家
};

/////////////////////////////////////////////////////
//房间通用管理类
/////////////////////////////////////////////////////
class RoomManager
{
private:
	std::mutex _no_password_mutex;
	//所有房间(包括已满、未满、要密码、不要密码)
	std::unordered_map<int64_t, std::shared_ptr<Room>> _rooms;
	//要输入密码，可入的房间
	std::unordered_map<int64_t, std::shared_ptr<Room>> _password_rooms; 
	//不要输入密码，可入的房间
	std::unordered_map<int64_t, std::shared_ptr<Room>> _no_password_rooms;
	//已满房间
	std::unordered_map<int64_t, std::shared_ptr<Room>> _full_rooms;
	
	//房间池
	std::unordered_map<int64_t, std::shared_ptr<Room>> _room_pool;
public:
	static RoomManager& Instance()
	{
		static RoomManager _instance;
		return _instance;
	}
	
	//创建房间
	int64_t CreateRoom();
	std::shared_ptr<Room> CreateRoom(const Asset::Room& room);
	//进入房间回调
	void OnCreateRoom(std::shared_ptr<Room> room);
	//获取房间
	std::shared_ptr<Room> Get(int64_t room_id);
	//获取可入房间
	std::shared_ptr<Room> GetAvailableRoom();
	//密码检查
	bool CheckPassword(int64_t room_id, std::string password);
};

#define RoomInstance RoomManager::Instance()

}
