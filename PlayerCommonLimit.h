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

		auto common_limit = player->Get().mutable_common_limit(); //玩家所有通用限制数据

		auto it = std::find_if(common_limit->elements().begin(), common_limit->elements().end(), [global_id](const Asset::PlayerCommonLimit_Element& limit){
					return global_id == limit.common_limit_id();
				});

		if (it != common_limit->elements().end()) return true; //已经存在

		auto element = common_limit->mutable_elements()->Add();
		element->set_common_limit_id(global_id);
		element->set_time_stamp(CommonTimerInstance.GetTime());

		return true;
	}

	//是否限制内
	bool IsCommonLimit(std::shared_ptr<Player> player, int64_t global_id)
	{
		if (!player || global_id == 0) return false; 

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

					if (Asset::CommonLimit_COOL_DOWN_CLEAR_TYPE_COOL_DOWN_CLEAR_TYPE_ZERO_TIME) //零点刷新
					{

					}
					else if (Asset::CommonLimit_COOL_DOWN_CLEAR_TYPE_COOL_DOWN_CLEAR_TYPE_WHEN_OPER) //上一次更新时间
					{

					}

				}
				break;
				
				case Asset::CommonLimit_COOL_DOWN_TYPE_COOL_DOWN_TYPE_WEEK:
				{

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
		auto common_limit = player->Get().mutable_common_limit(); //玩家所有通用限制数据

		for (const auto& element : common_limit->elements())
		{
			int64_t common_limit_id = element.common_limit_id();
			int32_t time_stamp = element.time_stamp();
			
			bool ret = check(common_limit_id, time_stamp, current_time); //检查是否可以清除

			if (ret) //删除
			{
				updated = true;
			}
		}

		return updated;
	}

};

#define PlayerCommonLimitInstance PlayerCommonLimit::Instance()

}
