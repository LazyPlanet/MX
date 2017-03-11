#pragma once

#include <memory>
#include <string>
#include "Asset.h"
#include "P_Header.h"

namespace Adoter
{

namespace pb = google::protobuf;

/*
 * 类说明：
 *
 * 所有物品基类
 *
 * */
class Item : public std::enable_shared_from_this<Item>
{
private:
	Asset::Item_CommonProp _common_prop; 
public:
	std::shared_ptr<Asset::Item_Item> _stuff;
public:
	Item();
	virtual ~Item();
	
	explicit Item(pb::Message* message /*Asset::Item_Xxx*/);
	explicit Item(std::shared_ptr<Asset::Item_Item>);
	explicit Item(Asset::Item_Item& item);

	//获取通用属性
	virtual Asset::Item_CommonProp& GetCommonProp() { return _common_prop; }
	//获取物品类型
	virtual Asset::ASSET_TYPE GetType() { return this->_stuff->type_t(); }
	//是否可使用	
	virtual bool CanUse();
	//返回值:消耗数量
	virtual int32_t Use(); 
};

}
