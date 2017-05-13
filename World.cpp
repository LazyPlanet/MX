#include "World.h"
#include "Protocol.h"
#include "Room.h"
#include "Game.h"
#include "PlayerMatch.h"
#include "PlayerName.h"
#include "MXLog.h"

namespace Adoter
{

const Asset::CommonConst* g_const = nullptr;

bool World::Load()
{
	//协议初始化：必须最先初始化
	if (!ProtocolInstance.Load()) 
	{
		//LOG(ERROR, "ProtocolInstance load error.");
		return false;
	}
	//数据初始化：必须最先初始化
	if (!AssetInstance.Load()) 
	{
		//LOG(ERROR, "AssetInstance load error.");
		return false;
	}
//////////////////////////////////////////////////
//不依赖顺序的数据初始化
//////////////////////////////////////////////////
	//游戏初始化
	if (!GameInstance.Load()) 
	{
		//LOG(ERROR, "GameInstance load error.");
		return false;
	}

	if (!NameInstance.Load())
	{
		DEBUG("%s:line:%d NameInstance.Load Failed\n", __func__, __LINE__);
		return false;
	}

//////////////////////////////////////////////////
//游戏内初始化
//////////////////////////////////////////////////

	//特殊ID定义表
	pb::Message* message = AssetInstance.Get(458753); 
	g_const = dynamic_cast<const Asset::CommonConst*>(message); 
	if (!g_const) 
	{
		//LOG(ERROR, "g_const is null.");
		return false;
	}

	//玩家匹配
	MatchInstance.DoMatch();

	return true;
}

//世界中所有刷新都在此(比如刷怪，拍卖行更新...)，当前周期为50MS.
void World::Update(int32_t diff)
{
	++_heart_count;

	MatchInstance.Update(diff);
}
	

}
