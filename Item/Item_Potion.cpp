#include "Item_Potion.h"

namespace Adoter
{

Item_Potion::~Item_Potion()
{
}

Item_Potion::Item_Potion()
{
}

Item_Potion::Item_Potion(Asset::Item_Potion* stuff)
{
	_stuff->set_stuff(stuff->SerializeAsString());
	_stuff->set_type_t(stuff->type_t());
}

bool Item_Potion::CanUse()
{
	return true;
}

int32_t Item_Potion::Use()
{
	return 0;
}	

}
