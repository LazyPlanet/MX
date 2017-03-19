#pragma once

#include <memory>
#include <functional>

#include "P_Header.h"
#include "Timer.h"

namespace Adoter
{
namespace pb = google::protobuf;

class PlayerCommonLimit : public std::enable_shared_from_this<PlayerCommonLimit> 
{
public:

	static PlayerCommonLimit& Instance()
	{
		static PlayerCommonLimit _instance;
		return _instance;
	}

	bool AddCommonLimit(std::shared_ptr<Player> player, int64_t global_id)
	{
		if (!player || global_id == 0) return false; 

		auto common_limit = player->GetMutableCommonLimit(); //玩家所有通用限制数据

		auto it = std::find_if(common_limit->elements().begin(), common_limit->elements().end(), [global_id](const Asset::PlayerCommonLimit_Element& limit){
					return global_id == limit.common_limit_id();
				});

		if (it != common_limit->elements().end()) 
		{
			auto mutable_it = const_cast<Asset::PlayerCommonLimit_Element*>(&(*it));
			mutable_it->set_count(it->count() + 1);
		}
		else
		{
			auto element = common_limit->mutable_elements()->Add();
			element->set_common_limit_id(global_id);
			element->set_time_stamp(CommonTimerInstance.GetTime());
			element->set_count(1);
		}

		auto message = make_unique<Asset::LogMessage>();
		message->set_common_limit(global_id); //限制
		LOG(TRACE, message.get()); //日志

		return true;
	}

	//是否限制内
	bool IsCommonLimit(std::shared_ptr<Player> player, int64_t global_id)
	{
		if (!player || global_id == 0) return false; //没有限制
		
		auto player_common_limit = player->GetMutableCommonLimit(); //玩家所有通用限制数据

		auto it = std::find_if(player_common_limit->elements().begin(), player_common_limit->elements().end(), 
				[global_id](const Asset::PlayerCommonLimit_Element& limit){
					return global_id == limit.common_limit_id();
				});

		if (it == player_common_limit->elements().end()) return false; //没有限制
			
		int32_t count = it->count();

		auto message = AssetInstance.Get(it->common_limit_id());
		if (!message) return false; //如果没有就不限制

		auto common_limit = dynamic_cast<Asset::CommonLimit*>(message);
		if (!common_limit) return false; 

		if (count < common_limit->max_count()) return false;

		return true;
	}
	
	//返回是否更新
	bool Update(std::shared_ptr<Player> player)
	{
		if (!player) return false; 

		auto check = [](int64_t global_id, int32_t time_stamp, int32_t current_time)->bool {

			const auto message = AssetInstance.Get(global_id);
			if (!message) return false; //如果没有就不限制

			const auto common_limit = dynamic_cast<const Asset::CommonLimit*>(message);
			if (!common_limit) return false; 

			Asset::CommonLimit_COOL_DOWN_CLEAR_TYPE clear_type = common_limit->cool_down_clear_type();

			switch (common_limit->cool_down_type())
			{
				case Asset::CommonLimit_COOL_DOWN_TYPE_COOL_DOWN_TYPE_DAY:
				{
					if (CommonTimerInstance.IsSameDay(time_stamp, current_time)) return false; //是一天，显然不用更新

					if (clear_type == Asset::CommonLimit_COOL_DOWN_CLEAR_TYPE_COOL_DOWN_CLEAR_TYPE_ZERO_TIME) //按照零点刷新
					{
						return true;
					}
					else if (clear_type == Asset::CommonLimit_COOL_DOWN_CLEAR_TYPE_COOL_DOWN_CLEAR_TYPE_WHEN_OPER) //按照上一次更新时间刷新
					{
						if (current_time >= time_stamp + 24 * 3600) return true; //24小时
					}
				}
				break;
				
				case Asset::CommonLimit_COOL_DOWN_TYPE_COOL_DOWN_TYPE_WEEK:
				{
					if (CommonTimerInstance.IsSameWeek(time_stamp, current_time)) return false; //是同一周，显然不用更新

					if (clear_type == Asset::CommonLimit_COOL_DOWN_CLEAR_TYPE_COOL_DOWN_CLEAR_TYPE_ZERO_TIME) //按照零点刷新
					{
						return true;
					}
					else if (clear_type == Asset::CommonLimit_COOL_DOWN_CLEAR_TYPE_COOL_DOWN_CLEAR_TYPE_WHEN_OPER) //按照上一次更新时间刷新
					{
						if (current_time >= time_stamp + 7 * 24 * 3600) return true; //7天
					}
				}
				break;
				
				case Asset::CommonLimit_COOL_DOWN_TYPE_COOL_DOWN_TYPE_MONTH:
				{

				}
				break;
				
				case Asset::CommonLimit_COOL_DOWN_TYPE_COOL_DOWN_TYPE_YEAR:
				{

				}
				break;
				
				case Asset::CommonLimit_COOL_DOWN_TYPE_COOL_DOWN_TYPE_NEVER:
				{

				}
				break;
				
				default:
				{

				}
				break;
			}

			return false;
		};
		
		bool updated = false;
		int32_t current_time = CommonTimerInstance.GetTime();
		auto common_limit = player->GetMutableCommonLimit(); //玩家所有通用限制数据

		const size_t size = common_limit->elements().size();

		for (size_t i = 0; i < size; ++i)
		{
			const auto& element = common_limit->elements(i);

			int64_t common_limit_id = element.common_limit_id();
			int32_t time_stamp = element.time_stamp();
			
			bool ret = check(common_limit_id, time_stamp, current_time); //检查是否可以清除

			if (ret) //删除，不用检查后面交换上的元素，下次心跳更新即可
			{
				common_limit->mutable_elements()->SwapElements(i, common_limit->elements().size() - 1);
				common_limit->mutable_elements()->RemoveLast();

				auto message = make_unique<Asset::LogMessage>();
				message->set_common_limit(common_limit_id); //限制
				message->set_content("clear it");
				LOG(TRACE, message.get()); //日志

				updated = true;
			}
		}

		return updated;
	}

};

#define CommonLimitInstance PlayerCommonLimit::Instance()

}
