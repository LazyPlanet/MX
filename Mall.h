#pragma once

#include <memory>
#include <functional>

#include "P_Header.h"
#include "Asset.h"
#include "Activity.h"


namespace Adoter
{
namespace pb = google::protobuf;

class Player;

////////////////////////////////////////////////////////////////////////////////
// 游戏内商城购买
////////////////////////////////////////////////////////////////////////////////
class Mall : public std::enable_shared_from_this<Mall> 
{

public:

	static Mall& Instance()
	{
		static Mall _instance;
		return _instance;
	}

	Asset::ERROR_CODE BuySomething(shared_ptr<Player> player, int64_t global_id)
	{
		auto message = AssetInstance.Get(global_id);
		if (!message) return Asset::ERROR_MALL_NOT_FOUND;

		Asset::Mall* mall = dynamic_cast<Asset::Mall*>(message);
		if (!mall) return Asset::ERROR_INNER;

		if (mall->activity_id() && !ActivityInstance.IsOpen(mall->activity_id())) return Asset::ERROR_ACTIVITY_NOT_OPEN; //活动尚未开启

		switch (mall->goods_type())
		{
			case Asset::Mall_GOODS_TYPE_GOODS_TYPE_HUANLEDOU: //购买欢乐豆，只能是钻石
			{
				int32_t diamond = mall->diamond();

				if (diamond <= 0 || diamond < player->GetDiamond())
				{
					return Asset::ERROR_DIAMOND_NOT_ENOUGH; //钻石不足
				}

				player->ConsumeDiamond(diamond);

				player->IncreaseHuanledou(mall->count());
			}
			break;
			
			case Asset::Mall_GOODS_TYPE_GOODS_TYPE_NORMAL_ITEM: //普通商品
			{
				int32_t diamond = mall->diamond();

				if (diamond <= 0 || diamond < player->GetDiamond())
				{
					return Asset::ERROR_DIAMOND_NOT_ENOUGH; //钻石不足
				}
				
				int32_t huanledou = mall->huanledou();

				if (huanledou <= 0 || huanledou < player->GetHuanledou())
				{
					return Asset::ERROR_BEANS_NOT_ENOUGH; //欢乐豆不足
				}
				
				player->ConsumeDiamond(diamond); //消耗钻石
				player->ConsumeHuanledou(huanledou); //消耗欢乐豆

				player->GainItem(mall->item_id(), mall->count()); //获取物品
			}
			break;
			
			default:
			{
				return Asset::ERROR_CLIENT_DATA; //Client数据错误
			}
			break;

		}

		return Asset::ERROR_SUCCESS;
	}
};

#define MallInstance Mall::Instance()

}


