#include <chrono>

#include "PlayerMatch.h"
#include "Player.h"
#include "Room.h"
#include "Timer.h"
#include "MXLog.h"

namespace Adoter
{
	
void PlayerMatch::Update(int32_t diff)
{
	_scheduler.Update(diff);
}

void PlayerMatch::Join(std::shared_ptr<Player> player, pb::Message* message)
{
	if (!player) return;

	auto player_id = player->GetID();

	auto enter_room = dynamic_cast<Asset::EnterRoom*>(message);
	if (!enter_room) return;

	Asset::ROOM_TYPE room_type = enter_room->room().room_type();
	
	auto enter_type = enter_room->enter_type();
		
	switch (room_type)
	{
		case Asset::ROOM_TYPE_XINSHOU:
		{
			if (enter_type == Asset::EnterRoom_ENTER_TYPE_ENTER_TYPE_ENTER)
			{
				_xinshou.emplace(player_id, player);
			}
			else
			{
				_xinshou.erase(player_id);
			}
		}
		break;
		
		case Asset::ROOM_TYPE_GAOSHOU:
		{
			if (enter_type == Asset::EnterRoom_ENTER_TYPE_ENTER_TYPE_ENTER)
			{
				_gaoshou.emplace(player_id, player);
			}
			else
			{
				_gaoshou.erase(player_id);
			}
		}
		break;
		
		case Asset::ROOM_TYPE_DASHI:
		{
			if (enter_type == Asset::EnterRoom_ENTER_TYPE_ENTER_TYPE_ENTER)
			{
				_dashi.emplace(player_id, player);
			}
			else
			{
				_dashi.erase(player_id);
			}
		}
		break;

		default:
		{
			auto log = make_unique<Asset::LogMessage>();
			log->set_player_id(player_id);
			log->set_type(Asset::PLAYER_MATCH);

			LOG(ERROR, log.get())
		}
		break;
	}
}
	
void PlayerMatch::DoMatch()
{
	auto match = [this](std::unordered_map<int64_t, std::shared_ptr<Player>>& player_list) {

		_scheduler.Schedule(std::chrono::seconds(3), [&](TaskContext task) {
			
			if (player_list.size() < 4) return;
				
			auto room_id = RoomInstance.CreateRoom();
			if (room_id <= 0) return;

			Asset::Room room;
			room.set_room_id(room_id);
			room.set_room_type(Asset::ROOM_TYPE_XINSHOU);

			auto local_room = RoomInstance.CreateRoom(room);
			if (!local_room) return;

			Asset::EnterRoom enter_room;
			enter_room.mutable_room()->CopyFrom(room);
			enter_room.set_enter_type(Asset::EnterRoom_ENTER_TYPE_ENTER_TYPE_ENTER); //进入房间

			bool match_success = true;

			for (auto it = player_list.begin(); it != std::next(player_list.begin(), 4); ++it)
			{
				auto ret = local_room->TryEnter(it->second); //玩家进入房间

				enter_room.set_error_code(ret); //是否可以进入场景//房间

				it->second->SendProtocol(enter_room); //提示Client是否成功

				if (Asset::ERROR_SUCCESS != ret) 
				{
					match_success = false; //理论上不应该出现，TODO：如果该玩家一直进不去，可能会导致后面玩家都进不去，需要处理

					auto log = make_unique<Asset::LogMessage>();
					log->set_player_id(it->second->GetID());
					log->set_type(Asset::PLAYER_MATCH);
					LOG(ERROR, log.get())
				}
			}

			if (match_success)
			{
				for (auto it = player_list.begin(); it != std::next(player_list.begin(), 4); ++it) player_list.erase(it); //删除队列中该几位玩家
			}
			
			task.Repeat(std::chrono::seconds(3)); //持续匹配
		});
	
	};

	match(_xinshou); //新手匹配
	match(_gaoshou); //高手匹配
	match(_dashi); //大师匹配
}

}
