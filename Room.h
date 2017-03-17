#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "Asset.h"
#include "Player.h"

namespace Adoter
{

class Player;
class Game;

class Room : public std::enable_shared_from_this<Room>
{
	static const int32_t max_player_count = 4;
private:
	std::shared_ptr<Asset::Room> _stuff;
	std::vector<std::shared_ptr<Game>> _games;
	std::unordered_map<int64_t, std::shared_ptr<Player>> _players; //房间中的玩家：按照进房间的顺序，东南西北
	std::shared_ptr<Player> _players_with_order[max_player_count]; //玩家数据：按照进房间的顺序，0->1->2->3...主要用于控制发牌和出牌顺序
public:
	explicit Room(Asset::Room room) {  _stuff = std::make_shared<Asset::Room>(room); }

	virtual int64_t GetID() { return _stuff->room_id(); }

	virtual void SetID(int64_t room_id) { return _stuff->set_room_id(room_id); }
public:
	Asset::ERROR_CODE TryEnter(std::shared_ptr<Player> player);
	void Enter(std::shared_ptr<Player> player);

	//void LeaveRoom(std::shared_ptr<Player> player);

	void OnCreated(); 

	bool IsFull() { return _players.size() >= (size_t)max_player_count; } //房间是否已满

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
	std::shared_ptr<Player> GetPlayerByOrder(int32_t player_index);
	//获取玩家的顺序
	int32_t GetPlayerOrder(int32_t player_id);
	//房间中是否有XX玩家
	bool HasPlayer(int64_t player_id);
	//删除玩家
	bool Remove(int64_t player_id);
	//获取下家
	std::shared_ptr<Player> GetNextPlayer(int64_t player_id);
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
