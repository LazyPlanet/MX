#include "Room.h"
#include "RedisManager.h"

#include <vector>
#include <algorithm>

namespace Adoter
{

void Room::EnterRoom(std::shared_ptr<Player> player)
{
	if (!player) return; 

	if (IsFull()) return;

	_players.insert(player->GetID());
}

void Room::LeaveRoom(std::shared_ptr<Player> player)
{
	if (!player) return;
	_players.erase(player->GetID());
}
	
void Room::OnCreated() 
{ 
	RoomInstance.OnCreateRoom(shared_from_this()); 
}

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
