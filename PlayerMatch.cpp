#include <chrono>

#include "PlayerMatch.h"
#include "Player.h"
#include "Room.h"

namespace Adoter
{

void PlayerMatch::Join(std::shared_ptr<Player> player, pb::Message* message)
{
	if (!player) return;

	auto player_id = player->GetID();

	auto enter_room = dynamic_cast<Asset::EnterRoom*>(message);
	if (!enter_room) return;

	Asset::ROOM_TYPE room_type = enter_room->room().room_type();
	/*
	bool cancel = enter_room->cancel();
		
	switch (room_type)
	{
		case Asset::ROOM_TYPE_XINSHOU:
		{
			if (cancel) _xinshou.erase(player_id);
			else _xinshou.emplace(player_id, player);
		}
		break;
		
		case Asset::ROOM_TYPE_GAOSHOU:
		{
			if (cancel) _gaoshou.erase(player_id);
			else _gaoshou.emplace(player_id, player);
		}
		break;
		
		case Asset::ROOM_TYPE_DASHI:
		{
			if (cancel) _dashi.erase(player_id);
			else _dashi.emplace(player_id, player);
		}
		break;

		default:
		{

		}
		break;
	}
	*/
}
	
void PlayerMatch::DoMatch()
{
	//新手房间匹配
	_scheduler.Schedule(std::chrono::seconds(3), [this](TaskContext task) {

		if (_xinshou.size() < 4) return;
			
		auto room_id = RoomInstance.CreateRoom();
		if (room_id <= 0) return;

		Asset::Room room;
		room.set_room_id(room_id);
		room.set_room_type(Asset::ROOM_TYPE_XINSHOU);

		auto local_room = RoomInstance.CreateRoom(room);
		if (!local_room) return;

		Asset::EnterRoom enter_room;
		enter_room.mutable_room()->CopyFrom(room);

		bool match_success = true;

		for (auto it = _xinshou.begin(); it != std::next(_xinshou.begin(), 4); ++it)
		{
			auto ret = local_room->TryEnter(it->second); //玩家进入房间

			enter_room.set_error_code(ret); //是否可以进入场景//房间

			it->second->SendProtocol(enter_room); //提示Client是否成功

			if (Asset::ERROR_SUCCESS != ret) match_success = false; //理论上不应该出现，TODO：如果该玩家一直进不去，可能会导致后面玩家都进不去，需要处理
		}

		if (match_success)
		{
			for (auto it = _xinshou.begin(); it != std::next(_xinshou.begin(), 4); ++it) _xinshou.erase(it); //删除队列中该几位玩家
		}
		
		//task.Repeat(std::chrono::seconds(3));
    });
}

}
