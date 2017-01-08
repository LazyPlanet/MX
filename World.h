#pragma once

#include "Asset.h"

#include <memory>

namespace Adoter 
{

/*
 * 类说明：
 *
 * 大世界
 *
 * 游戏中玩家公共空间，区别副本.
 *
 * 说明：有的游戏中称为场景(World)，场景分为大世界和副本.
 *
 * */

extern const Asset::CommonConst* g_const;

class World : public std::enable_shared_from_this<World>
{
private:
	bool _stopped = false;
	int64_t _heart_count = 0; //心跳记次
public:
	static World& Instance()
	{
		static World _instance;
		return _instance;
	}

	bool IsStopped() { return _stopped; }

	//世界中所有刷新都在此(比如刷怪，拍卖行更新...)，当前周期为50MS.
	void Update(int32_t diff);
	//加载所有
	bool Load();
};

#define WorldInstance World::Instance()

}
