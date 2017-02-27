#include "Room.h"
#include "RedisManager.h"

#include <vector>
#include <algorithm>

namespace Adoter
{

/////////////////////////////////////////////////////
//房间
/////////////////////////////////////////////////////
void Room::EnterRoom(std::shared_ptr<Player> player)
{
	if (!player) return; 

	if (IsFull()) return;

	_players.emplace(player->GetID(), player);
}

void Room::LeaveRoom(std::shared_ptr<Player> player)
{
	if (!player) return;

	_players.erase(player->GetID()); //玩家退出房间
}
	
void Room::OnPlayerOperate(std::shared_ptr<Player> player, Asset::GAME_OPER_TYPE oper_type)
{
	if (!player) return;

	BroadCast(nullptr, player->GetID());
}

void Room::BroadCast(pb::Message* message, int64_t exclude_player_id)
{
	for (auto player : _players)
	{
		if (exclude_player_id == player.second->GetID()) continue;

		player.second->SendProtocol(message);
	}
}
	
void Room::OnCreated() 
{ 
	RoomInstance.OnCreateRoom(shared_from_this()); 
}

/////////////////////////////////////////////////////
//房间通用管理类
/////////////////////////////////////////////////////
std::shared_ptr<Room> RoomManager::Get(int64_t room_id)
{
	auto it = _rooms.find(room_id);
	if (it == _rooms.end()) return nullptr;
	return it->second;
}
	
bool RoomManager::CheckPassword(int64_t room_id, std::string password)
{
	auto room = Get(room_id);
	if (!room) return false;

	return true;
}

int64_t RoomManager::CreateRoom()
{
	std::shared_ptr<Redis> redis = std::make_shared<Redis>();
	int64_t room_id = redis->CreateRoom();
	return room_id;
}

void RoomManager::OnCreateRoom(std::shared_ptr<Room> room)
{
	if (_rooms.find(room->GetID()) != _rooms.end()) return;

	_rooms.emplace(room->GetID(), room);
}

std::shared_ptr<Room> RoomManager::GetAvailableRoom()
{
	std::lock_guard<std::mutex> lock(_no_password_mutex);

	if (_no_password_rooms.size())
	{
		return _no_password_rooms.begin()->second; //取一个未满的房间
	}

	return nullptr;
}

}
