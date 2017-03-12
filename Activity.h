#pragma once

#include <memory>
#include <functional>
//#include <unordered_map>
//#include <unordered_set>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/conversion.hpp"

#include "Timer.h"
#include "P_Header.h"

namespace Adoter
{
namespace pb = google::protobuf;

class Activity : public std::enable_shared_from_this<Activity> 
{
	std::unordered_map<int64_t/*global_id*/, pb::Message*> _list;

public:

	static Activity& Instance()
	{
		static Activity _instance;
		return _instance;
	}
	
	/*
	bool Load()
	{
		std::unordered_set<pb::Message*> messages = AssetInstance.GetMessagesByType(Asset::ASSET_TYPE_ACTIVITY);

		for (auto message : messages)
		{
			const Asset::Activity* activity = dynamic_cast<const Asset::Activity*>(message); 
			if (!activity) return false; //有一个出错即退出

			_list.emplace(activity->common_prop().global_id(), message);
		}

		return true;
	}
	*/
		
	bool IsOpen(int64_t global_id)
	{
		auto message = AssetInstance.Get(global_id);
		if (!message) return true; //如果没有配置，则认为是开

		const Asset::Activity* activity = dynamic_cast<const Asset::Activity*>(message);
		if (!activity) return true;

		boost::posix_time::ptime start_time(boost::posix_time::time_from_string(activity->start_time()));
		auto start_time_t = boost::posix_time::to_time_t(start_time);

		boost::posix_time::ptime stop_time(boost::posix_time::time_from_string(activity->stop_time()));
		auto stop_time_t = boost::posix_time::to_time_t(stop_time);
		
		std::time_t cur_t = CommonTimerInstance.GetTime();
		if (cur_t >= start_time_t && cur_t <= stop_time_t) return true;

		return false;
	}
};

#define ActivityInstance Activity::Instance()

}

