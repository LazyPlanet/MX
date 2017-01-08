#include "Hoster.h"
#include "Protocol.h"
#include "CommonUtil.h"
#include "RedisManager.h"

#include <iostream>
#include <hiredis.h>

namespace Adoter
{

/*
 * 居民
 * 
 * */

Hoster::Hoster(Asset::Hoster& hoster)
{
	this->_stuff = hoster;	//居民属性
	this->_super_common_prop = hoster.human_prop(); //基类公共属性
}

Hoster::Hoster(Asset::Hoster* hoster)
{
	this->_stuff = *hoster; //居民属性
	this->_super_common_prop = hoster->human_prop(); //基类公共属性
}

Hoster::Hoster(int64_t hoster_id) 
{
	pb::Message* message = AssetInstance.Get(hoster_id); //数据初始化 
	if (message)
	{
		Asset::Hoster* hoster = dynamic_cast<Asset::Hoster*>(message);
		if (hoster) 
		{
			this->_stuff = *hoster; //居民属性
			this->_super_common_prop = hoster->human_prop(); //基类公共属性
		}
	}
}

bool Hoster::HandleMessage(const Asset::MsgItem& item)
{
	switch (item.type())
	{
		//进入视野
		case Asset::MSG_TYPE_AOI_ENTER:
		{
			if (item.sender() == item.receiver()) return false; //这种协议不处理
			Asset::CommonProp common_prop;
			//if (!common_prop.ParseFromString(item.content())) return;
			//if (!IsInSight(common_prop.position())) return;	//如果看不到该实体，则退出
		}
		break;
		//离开视野
		case Asset::MSG_TYPE_AOI_LEAVE:
		{
			if (item.sender() == item.receiver()) return false; //这种协议不处理
			Asset::CommonProp common_prop;
		}
		break;
		//移动	
		case Asset::MSG_TYPE_AOI_MOVE:
		{
			if (item.sender() == item.receiver()) return false; //这种协议不处理
			Asset::CommonProp common_prop;
			//if (!common_prop.ParseFromString(item.content())) return;
		}
		break;
		
		default:
		{
			std::cout << __func__ << ":Erroooooooooooooooooooooooooor:No handler, type:" << item.type() << std::endl;
		}
		break;
	}
	
	//std::cout << __func__ << " of Hoster call:sender:" << item.sender() << ", receiver:" << item.receiver() << ", type:" << item.type() << std::endl;
	return true;
}
	
bool Hoster::Update(int32_t heart_count) //50MS
{
	auto curr_scene = GetScene();
	if (!curr_scene) return false;

	if (heart_count % 20 == 0) //1S
	{
		int32_t speed = this->_stuff.human_prop().speed();
		Asset::Vector3 pos;
		int32_t x = CommonUtil::Random(-speed, speed);	
		int32_t z = CommonUtil::Random(-speed, speed);	
		//随机移动
		Asset::Vector3 position = this->_stuff.human_prop().position();
		position.set_x(position.x() + x);
		position.set_z(position.z() + z);

		//std::cout << "---------ID:" << this->_ID << " POS :" << position.x() << " " << position.z() << std::endl;

		this->StepMove(position);
	}
	return true;
}

/*
 * 居民管理
 * 
 * */

bool HosterManager::Load()
{
	auto& hosters = AssetInstance.GetMessagesByType(Asset::ASSET_TYPE_HOSTER);
	for (auto m : hosters)
	{
		Asset::Hoster* h = dynamic_cast<Asset::Hoster*>(m);
		if (!h) return false;

		int64_t scene_id = h->human_prop().scene_id();
		auto scene = SceneInstance.Get(scene_id);
		if (!scene)
		{
			std::cerr << "Hoster:" << h->common_prop().global_id() << " whoes scene id is "<< scene_id << " is invalid." << std::endl;
			continue;
		}
		
		auto hoster = std::make_shared<Hoster>(h);
		hoster->SetID(h->common_prop().global_id()); //比较特殊，具备玩家和资源的双重属性，其全局ID采用资源ID，不重新分配.
		hoster->OnBirth(scene_id); //居民出生

		_hosters.emplace(hoster);
	}
	return true;
}

bool HosterManager::Update(int32_t diff)
{
	for (auto hoster : _hosters)
	{
		hoster->Update(diff);
	}
	return true;
}

}
