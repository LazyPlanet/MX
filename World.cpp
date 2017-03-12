#include "World.h"
#include "Protocol.h"
#include "Room.h"
#include "Log.h"
#include "Game.h"

namespace Adoter
{

const Asset::CommonConst* g_const = nullptr;

bool World::Load()
{
	//协议初始化：必须最先初始化
	if (!ProtocolInstance.Load()) 
	{
		LOG(ERROR, "ProtocolInstance load error.");
		return false;
	}
	//数据初始化：必须最先初始化
	if (!AssetInstance.Load()) 
	{
		LOG(ERROR, "AssetInstance load error.");
		return false;
	}
	//其他初始化
	if (!GameInstance.Load()) 
	{
		LOG(ERROR, "GameInstance load error.");
		return false;
	}

	pb::Message* message = AssetInstance.Get(458753); //唯一写死ID
	g_const = dynamic_cast<const Asset::CommonConst*>(message); 
	if (!g_const) 
	{
		LOG(ERROR, "g_const is null.");
		return false;
	}

	return true;
}

//世界中所有刷新都在此(比如刷怪，拍卖行更新...)，当前周期为50MS.
void World::Update(int32_t diff)
{
	++_heart_count;
}
	

}
