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
private:
	std::shared_ptr<Asset::Room> _stuff;
	std::vector<std::shared_ptr<Game>> _games;
	std::unordered_map<int64_t, std::shared_ptr<Player>> _players; //房间中的玩家
public:
	explicit Room(Asset::Room& room) {  _stuff = std::make_shared<Asset::Room>(room); }

	virtual int64_t GetID() { return _stuff->room_id(); }

	virtual void SetID(int64_t room_id) { return _stuff->set_room_id(room_id); }
public:
	void EnterRoom(std::shared_ptr<Player> player);
	void LeaveRoom(std::shared_ptr<Player> player);

	void OnCreated(); 

	bool IsFull() { return _players.size() >= 4; }

	bool CanStarGame();

	void OnPlayerOperate(std::shared_ptr<Player> player, pb::Message* message);

	void BroadCast(pb::Message* message, int64_t exclude_player_id = 0);
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
