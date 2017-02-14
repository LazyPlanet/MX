#include "Item_Equipment.h"

namespace Adoter
{

Item_Equipment::~Item_Equipment()
{
}

Item_Equipment::Item_Equipment()
{
}

//Item_Equipment::Item_Equipment(Asset::Item_Equipment* stuff)
//{
//	_stuff->set_stuff(stuff->SerializeAsString());
//	_stuff->set_type_t(stuff->type_t());
//}

bool Item_Equipment::CanUse()
{
	return true;
}

int32_t Item_Equipment::Use()
{
	return 0;
}	

}
