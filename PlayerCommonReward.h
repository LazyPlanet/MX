#pragma once

#include <memory>
#include <functional>

#include "P_Header.h"

namespace Adoter
{
namespace pb = google::protobuf;

class PlayerCommonReward : public std::enable_shared_from_this<PlayerCommonReward> 
{
	Asset::CommonReward _reward;
public:
	PlayerCommonReward(const Asset::CommonReward& reward){
		_reward.CopyFrom(reward);
	}

	bool DeliverReward(std::shared_ptr<Player> player, int64_t global_id)
	{
		const auto message = AssetInstance.Get(global_id);
		if (!message) return false;

		const auto common_reward = dynamic_cast<const Asset::CommonReward*>(message);
		if (!common_reward) return false;

		for (const auto& reward : common_reward->rewards())
		{
			switch (reward.reward_type())
			{
				case Asset::CommonReward_REWARD_TYPE_REWARD_TYPE_DIAMOND:
				{
					int32_t count = reward.count();
					int64_t common_limit_id = reward.common_limit_id();

					//g_player->Consum

				}
				break;

				case Asset::CommonReward_REWARD_TYPE_REWARD_TYPE_HUANLEDOU:
				{

				}
				break;
				
				case Asset::CommonReward_REWARD_TYPE_REWARD_TYPE_ITEM:
				{

				}
				break;

				default:
				{

				}
				break;
			}
		}
	}

};

}
