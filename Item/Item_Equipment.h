#pragma once

#include "Item.h"
#include "P_Header.h"

namespace Adoter
{

class Item_Equipment : public Item
{
private:
	Asset::ItemEquipment stuff_extra;
public:
	Item_Equipment();
	~Item_Equipment();
	
	//explicit Item_Equipment(Asset::Item_Equipment* stuff);
	
	virtual bool CanUse() override;     
	virtual int32_t Use() override; 
};

}
