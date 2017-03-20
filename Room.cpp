#include <vector>
#include <algorithm>

#include <boost/asio.hpp>

#include "Room.h"
#include "Game.h"
#include "RedisManager.h"

namespace Adoter
{

/////////////////////////////////////////////////////
//房间
/////////////////////////////////////////////////////

Asset::ERROR_CODE Room::TryEnter(std::shared_ptr<Player> player)
{
	if (!player || IsFull()) return Asset::ERROR_ROOM_IS_FULL;

	return Asset::ERROR_SUCCESS;
}

void Room::Enter(std::shared_ptr<Player> player)
{
	if (TryEnter(player) != Asset::ERROR_SUCCESS) return; //进入房间之前都需要做此检查，理论上不会出现

	_players.push_back(player); //进入房间

	player->SetPosition((Asset::POSITION_TYPE)_players.size()); //设置位置

	/*

	//广播给其他玩家
	Asset::CommonProperty common_prop;
	common_prop.set_reason_type(Asset::CommonProperty_SYNC_REASON_TYPE_SYNC_REASON_TYPE_ENTER_ROOM);
	common_prop.set_player_id(player->GetID());
	common_prop.mutable_common_prop()->CopyFrom(player->CommonProp());

	BroadCast(common_prop, player->GetID());

	*/
	SyncRoom(); //同步当前房间内玩家数据
}

/*
void Room::LeaveRoom(std::shared_ptr<Player> player)
{
	if (!player) return;

	_players.erase(player->GetID()); //玩家退出房间
}
*/

std::shared_ptr<Player> Room::GetHoster()
{
	if (_players.size() <= 0) return nullptr;

	return *_players.begin(); //房间里面的一个人就是房主
}

bool Room::IsHoster(int64_t player_id)
{
	auto host = GetHoster();
	if (!host) return false;

	return host->GetID() == player_id;
}

/*
int32_t Room::GetPlayerOrder(int32_t player_id)
{
	for (int player_index = 0; player_index < max_player_count; ++player_index)
	{
		if (player_id == _players_with_order[player_index]->GetID()) return player_index;
	}

	return -1; //返回一个非法的值
}
*/

std::shared_ptr<Player> Room::GetPlayer(int64_t player_id)
{
	for (auto player : _players)
	{
		if (player->GetID() == player_id) return player;
	}

	return nullptr;
}

/*
std::shared_ptr<Player> Room::GetPlayerByOrder(int32_t player_index)
{
	if (player_index >= max_player_count || player_index < 0) return nullptr;

	return _players_with_order[player_index];
}

bool Room::HasPlayer(int64_t player_id)
{
	return _players.find(player_id) != _players.end();
}
*/

void Room::OnPlayerOperate(std::shared_ptr<Player> player, pb::Message* message)
{
	if (!player) return;
	
	auto game_operate = dynamic_cast<Asset::GameOperation*>(message);
	if (!game_operate) return;
			
	BroadCast(game_operate); //广播玩家操作
	
	switch(game_operate->oper_type())
	{
		case Asset::GAME_OPER_TYPE_START: //开始游戏：其实是个准备
		{
			if (!CanStarGame()) return;

			auto game = std::make_shared<Game>();

			game->Init(shared_from_this()); //洗牌

			game->Start(_players); //开始游戏

			GameInstance.OnCreateGame(game);
		}
		break;

		case Asset::GAME_OPER_TYPE_LEAVE: //离开游戏
		{
			Remove(player->GetID()); //玩家退出房间
		}
		break;
		
		case Asset::GAME_OPER_TYPE_KICKOUT: //踢人
		{
			Remove(game_operate->destination_player_id()); //玩家退出房间
		}
		break;
		
		default:
		{
		}
		break;
	}
}

bool Room::Remove(int64_t player_id)
{
	for (auto it = _players.begin(); it != _players.end(); ++it)
	{
		if ((*it)->GetID() != player_id) continue;

		_players.erase(it); //删除玩家
		return true;
	}

	return false;
}

void Room::BroadCast(pb::Message* message, int64_t exclude_player_id)
{
	if (!message) return;

	for (auto player : _players)
	{
		if (exclude_player_id == player->GetID()) continue;

		player->SendProtocol(message);
	}
}
	
void Room::BroadCast(pb::Message& message, int64_t exclude_player_id)
{
	BroadCast(&message, exclude_player_id);
}
	
void Room::SyncRoom()
{
	Asset::RoomInformation message;
	
	for (auto player : _players)
	{
		auto p = message.mutable_player_list()->Add();
		p->set_position(player->GetPosition());
		p->mutable_common_prop()->CopyFrom(player->CommonProp());
	}

	BroadCast(message);
}

void Room::OnCreated() 
{ 
	RoomInstance.OnCreateRoom(shared_from_this()); 
}
	
bool Room::CanStarGame()
{
	if (_players.size() != 4) return false;

	for (auto player : _players)
	{
		if (!player->IsReady()) return false; //需要所有玩家都是准备状态
	}

	return true;
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
	
std::shared_ptr<Room> RoomManager::CreateRoom(const Asset::Room& room)
{
	auto locate_room = std::make_shared<Room>(room);
	locate_room->OnCreated();

	RoomInstance.OnCreateRoom(locate_room); //房间管理

	return locate_room;
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
