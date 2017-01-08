#pragma once

#include "Item.h"

namespace Adoter
{

class Item_Potion : public Item
{
public:
	Item_Potion();
	~Item_Potion();
	
	explicit Item_Potion(Asset::Item_Potion* stuff);
	
	virtual bool CanUse() override; //是否可使用    
	virtual int32_t Use() override; //返回值：消耗数量
};

}
