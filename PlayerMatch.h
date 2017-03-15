#pragma once

#include <memory>
#include <functional>
#include <unordered_map>

#include "P_Header.h"
#include "TaskScheduler.h"

namespace Adoter
{
namespace pb = google::protobuf;

class Player;

class PlayerMatch : public std::enable_shared_from_this<PlayerMatch>   
{
	std::unordered_map<int64_t, std::shared_ptr<Player>> _xinshou; 
	std::unordered_map<int64_t, std::shared_ptr<Player>> _gaoshou;
	std::unordered_map<int64_t, std::shared_ptr<Player>> _dashi;

	TaskScheduler _scheduler;
public:

};

}
