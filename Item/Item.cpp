#include "Item.h"

namespace Adoter
{

/////////////////////////////////////////////////////
//
//如果物品不带逻辑，可以不用创建该物品的C++类
//
//直接利用基类管理即可
//
/////////////////////////////////////////////////////


Item::~Item()
{
}

Item::Item()
{
	//初始化一个资源
	_stuff = std::make_shared<Asset::Item_Item>();
}

Item::Item(pb::Message* message /*Asset::Item_Xxx*/) : Item()/*委托构造函数*/
{
	const pb::FieldDescriptor* type_field = message->GetDescriptor()->FindFieldByName("type_t");
	_stuff->set_type_t((Asset::ASSET_TYPE)type_field->default_value_enum()->number()); //加载资源已经处理
	//设置所包装物品配置数据
	_stuff->set_stuff(message->SerializeAsString());
	//初始化物品公共属性
	const pb::FieldDescriptor* prop_field = message->GetDescriptor()->FindFieldByName("item_common_prop"); //物品公共属性变量
	const pb::Message& const_item_common_prop = message->GetReflection()->GetMessage(*message, prop_field);
	try{
		pb::Message& item_common_prop = const_cast<pb::Message&>(const_item_common_prop);
		this->_common_prop = dynamic_cast<Asset::Item_CommonProp&>(item_common_prop);
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

Item::Item(Asset::Item_Item& item)
{
	_stuff->CopyFrom(item);
}

Item::Item(std::shared_ptr<Asset::Item_Item> item)
{
	_stuff = item;
}
	
bool Item::CanUse() 
{
	int min_level_limit = _common_prop.min_level_limit();
	int max_level_limit = _common_prop.max_level_limit();
	std::cout << "物品级别限制：最小(" << min_level_limit << ") 最大(" << max_level_limit << ")" << std::endl;
	return true; 
}

//返回值代表消耗物品数量
int32_t Item::Use() 
{ 
	if (!CanUse()) return 0;
	return 1; 
}

}
