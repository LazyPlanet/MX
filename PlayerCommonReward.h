#pragma once

#include <memory>
#include <functional>

#include "P_Header.h"
#include "MXLog.h"

namespace Adoter
{
namespace pb = google::protobuf;

class PlayerCommonReward : public std::enable_shared_from_this<PlayerCommonReward> 
{
	//Asset::CommonReward _reward;
public:
	/*
	PlayerCommonReward(const Asset::CommonReward& reward){
		_reward.CopyFrom(reward);
	}*/
	
	static PlayerCommonReward& Instance()
	{
		static PlayerCommonReward _instance;
		return _instance;
	}

	bool DeliverReward(std::shared_ptr<Player> player, int64_t global_id)
	{
		const auto message = AssetInstance.Get(global_id);
		if (!message) return false;

		const auto common_reward = dynamic_cast<const Asset::CommonReward*>(message);
		if (!common_reward) return false;

		for (const auto& reward : common_reward->rewards())
		{
			int64_t common_limit_id = reward.common_limit_id();
			if (player->IsCommonLimit(common_limit_id)) return false; //该奖励已经超过领取限制

			int32_t count = reward.count();

			switch (reward.reward_type())
			{
				case Asset::CommonReward_REWARD_TYPE_REWARD_TYPE_DIAMOND:
				{
					player->IncreaseDiamond(count);

				}
				break;

				case Asset::CommonReward_REWARD_TYPE_REWARD_TYPE_HUANLEDOU:
				{
					player->IncreaseHuanledou(count);
				}
				break;
				
				case Asset::CommonReward_REWARD_TYPE_REWARD_TYPE_ITEM:
				{
					player->GainItem(reward.item_id(), count);
				}
				break;

				default:
				{

				}
				break;
			}

			auto message = make_unique<Asset::LogMessage>();
			message->set_common_reward(global_id); //奖励
			message->set_common_limit(common_limit_id); //限制
			LOG(TRACE, message.get()); //日志
			
			player->AddCommonLimit(common_limit_id); 
		}
		return true;
	}

};

#define CommonRewardInstance PlayerCommonReward::Instance()

}
