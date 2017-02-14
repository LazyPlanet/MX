#include "Player.h"
#include "Protocol.h"
#include "CommonUtil.h"
#include "RedisManager.h"

#include <iostream>
#include <hiredis.h>

namespace Adoter
{

Player::Player()
{
	//协议默认处理函数
	_method = std::bind(&Player::DefaultMethod, this, std::placeholders::_1);
	//协议处理回调初始化
	AddHandler(Asset::META_TYPE_C2S_LOGIN, std::bind(&Player::CmdLogin, this, std::placeholders::_1));
	AddHandler(Asset::META_TYPE_C2S_ENTER_GAME, std::bind(&Player::CmdEnterGame, this, std::placeholders::_1));
	AddHandler(Asset::META_TYPE_C2S_CREATE_ROOM, std::bind(&Player::CmdCreateRoom, this, std::placeholders::_1));
	AddHandler(Asset::META_TYPE_C2S_ENTER_ROOM, std::bind(&Player::CmdEnterRoom, this, std::placeholders::_1));
}

Player::Player(int64_t player_id, std::shared_ptr<WorldSession> session) : Player()/*委派构造函数*/
{
	this->SetID(player_id);	//设置玩家ID
	this->_session = session; //地址拷贝
}

int32_t Player::Load()
{
	//加载数据库
	std::shared_ptr<Redis> redis = std::make_shared<Redis>();
	std::string stuff = redis->GetPlayer(GetID()); //不能用引用
	if (stuff == "")
	{
		std::cout << __func__ << " error, not found player_id:" << GetID() << std::endl;
		return 0;
	}
	//初始化结构数据
	this->_stuff.ParseFromString(stuff);
	//初始化包裹，创建角色或者增加包裹会调用一次
	do {
		const pb::EnumDescriptor* enum_desc = Asset::INVENTORY_TYPE_descriptor();
		if (!enum_desc) return 0;
		int32_t curr_inventories_size = _stuff.inventory().inventories_size(); 
		if (curr_inventories_size == enum_desc->value_count() - 1) break; 
		for (int inventory_index = curr_inventories_size; inventory_index < enum_desc->value_count() - 1; ++inventory_index)
		{
			auto inventory = _stuff.mutable_inventory()->mutable_inventories()->Add(); //增加新包裹，且初始化数据
			inventory->set_inventory_type((Asset::INVENTORY_TYPE)(inventory_index + 1));
			int inventory_size = enum_desc->value(inventory_index)->options().GetExtension(Asset::inventory_size); //默认格子数：如果修改需要清档
			if (inventory->items_size() >= inventory_size) continue;
			inventory->mutable_items()->Reserve(inventory_size); //设置默认格子数，即包裹大小
			//提示信息
			const pb::EnumValueDescriptor *enum_value = enum_desc->value(inventory_index);
			if (!enum_value) break;
			std::cout << "Add inventory type:" << enum_value->name() << ", size:" << inventory_size << std::endl;
		}
	} while(false);

	return 0;
}

int32_t Player::Save()
{
	//存入数据库
	std::string stuff = this->_stuff.SerializeAsString();
	std::shared_ptr<Redis> redis = std::make_shared<Redis>();
	redis->SavePlayer(GetID(), stuff);

	return 0;
}

int32_t Player::CmdLogin(pb::Message* message)
{
	if (Load()) return 1;
	//发送数据给客户端
	SendPlayer();
	return 0;
}

int32_t Player::CmdLogout(pb::Message* message)
{
	//非存盘数据
	this->_stuff.mutable_player_prop()->Clear(); 
	Save();	//存盘
	OnLeaveRoom(); //房间处理
	return 0;
}
	
int32_t Player::CmdEnterGame(pb::Message* message)
{
	OnEnterGame();
	return 0;
}

int32_t Player::OnEnterGame() 
{
	if (Load()) return 1;
	SendPlayer(); //发送数据给玩家
	return 0;
}

int32_t Player::CmdLeaveRoom(pb::Message* message)
{
	OnLeaveRoom(); //房间处理
	return 0;
}

void Player::SendPlayer()
{
	Asset::PlayerInfo player_info;
	player_info.mutable_player()->CopyFrom(this->_stuff);
	this->SendProtocol(player_info);
}

int32_t Player::CmdCreateRoom(pb::Message* message)
{
	Asset::CreateRoom* create_room = dynamic_cast<Asset::CreateRoom*>(message);
	if (!create_room) return 1;

	int64_t room_id = RoomInstance.CreateRoom();
	create_room->mutable_room()->set_room_id(room_id);

	Asset::Room asset_room;
	asset_room.set_room_id(room_id);
	asset_room.set_room_name(create_room->room().room_name());
	asset_room.set_enter_password(create_room->room().enter_password());

	Room room(asset_room);
	room.OnCreated();

	OnCreateRoom(create_room);
	return 0;
}

void Player::OnCreateRoom(Asset::CreateRoom* message)
{
	SendResponse(message);
}

int32_t Player::CmdEnterRoom(pb::Message* message) 
{
	Asset::EnterRoom* enter_room = dynamic_cast<Asset::EnterRoom*>(message);
	if (!enter_room) return 1;

	int64_t room_id = enter_room->room_id();
	if (room_id == 0) //随机进入
	{
		auto room = RoomInstance.GetAvailableRoom();
		if (!room) 
		{
			AlterError(Asset::ERROR_ROOM_NOT_AVAILABLE);
			return 5; //没有合适的房间
		}

		room_id = room->GetID();
	}
	else
	{
		auto room = RoomInstance.Get(room_id);
		if (!room) 
		{
			AlterError(Asset::ERROR_ROOM_NOT_FOUNT);
			return 2; 
		}

		if (room->IsFull()) 
		{
			AlterError(Asset::ERROR_ROOM_FULL);
			return 3;
		}

		std::string password = room->GetPassWord();
		if (password != "" && password != enter_room->enter_password()) 
		{
			AlterError(Asset::ERROR_ROOM_PASSWORD);
			return 4;
		}
	}
	OnEnterRoom(room_id); //通知同房间玩家，同时在房间中初始化该玩家
	return 0;
}

void Player::OnEnterRoom(int64_t room_id)
{
	_locate_room = RoomInstance.Get(room_id);
	if (!_locate_room) return; //非法的房间 

	SetRoomID(room_id); //设置当前房间ID
	GetRoom()->EnterRoom(shared_from_this());
	//向房间玩家发送公共数据
	BroadCastCommonProp(Asset::MSG_TYPE_AOI_ENTER);
}

bool Player::HandleMessage(const Asset::MsgItem& item)
{
	switch (item.type())
	{
	}

	return true;
}

void Player::SendMessage(Asset::MsgItem& item)
{
	DispatcherInstance.SendMessage(item);
}	

void Player::SendProtocol(pb::Message& message)
{
	const pb::FieldDescriptor* field = message.GetDescriptor()->FindFieldByName("type_t");
	if (!field) return;
	
	int type_t = field->default_value_enum()->number();
	if (!Asset::META_TYPE_IsValid(type_t)) return;	//如果不合法，不检查会宕线
	
	Asset::Meta meta;
	meta.set_type_t((Asset::META_TYPE)type_t);
	meta.set_stuff(message.SerializeAsString());
	std::string content = meta.SerializeAsString();
	this->GetSession()->AsyncSend(content);
}

void Player::SendResponse(pb::Message* message)
{
	if (!message) return;

	const pb::FieldDescriptor* field = message->GetDescriptor()->FindFieldByName("type_t");
	if (!field) return;
	
	int type_t = field->default_value_enum()->number();
	if (!Asset::META_TYPE_IsValid(type_t)) return;	//如果不合法，不检查会宕线
	
	Asset::CommonOperationResponse response;
	response.set_client_type_t((Asset::META_TYPE)type_t);
	SendProtocol(response);
}

void Player::SendToRoomers(pb::Message& message) 
{
}

//玩家心跳周期为10MS，如果该函数返回FALSE则表示掉线
bool Player::Update()
{
	++_heart_count; //心跳

	if (_heart_count % 6000 == 0) //1MIN
	{
		std::cout << "===================Time has gone 1min, i am id:" << GetID() << std::endl;
	}
	return true;
}
	
int32_t Player::DefaultMethod(pb::Message* message)
{
	const pb::FieldDescriptor* field = message->GetDescriptor()->FindFieldByName("type_t");
	if (!field) 
	{
		std::cout << __func__ << ":Could not found type_t of received message." << std::endl;
		return 1;
	}
	const pb::EnumValueDescriptor* enum_value = message->GetReflection()->GetEnum(*message, field);
	if (!enum_value) return 2;

	const std::string& enum_name = enum_value->name();
	std::cout << __func__ << ":Could not found call back, message type is: " << enum_name.c_str() << std::endl;
	return 0;
}

bool Player::HandleProtocol(int32_t type_t, pb::Message* message)
{
	CallBack& callback = GetMethod(type_t); 
	callback(std::forward<pb::Message*>(message));	
	return true;
}

bool Player::GainItem(int64_t global_item_id)
{
	pb::Message* asset_item = AssetInstance.Get(global_item_id); //此处取出来的必然为合法ITEM.
	if (!asset_item) return false;

	Item* item = new Item(asset_item);
	GainItem(item);
	return true;
}

bool Player::GainItem(Item* item)
{
	if (!item) return false;

	const Asset::Item_CommonProp& common_prop = item->GetCommonProp(); 
	if (!PushBackItem(common_prop.inventory(), item)) return false;
	return true;
}

bool Player::PushBackItem(Asset::INVENTORY_TYPE inventory_type, Item* item)
{
	if (!item) return false;

	const pb::EnumDescriptor* enum_desc = Asset::INVENTORY_TYPE_descriptor();
	if (!enum_desc) return false;

	int inventory_size = enum_desc->value(inventory_type)->options().GetExtension(Asset::inventory_size); //默认格子数

	Asset::Inventories_Inventory* inventory = _stuff.mutable_inventory()->mutable_inventories(inventory_type); 
	if (!inventory) return false;

	if (inventory->items_size() >= inventory_size) return false; //超过包裹最大限制

	auto item_toadd = inventory->mutable_items()->Add();
	item_toadd->CopyFrom(item->GetCommonProp());
	return true;
}

void Player::BroadCastCommonProp(Asset::MSG_TYPE type)       
{
	Asset::MsgItem item; //消息数据
	item.set_type(type);
	item.set_sender(GetID());
	this->BroadCast(item); //通知给房间玩家
}

void Player::OnLeaveRoom()
{
	if (!_locate_room) return; 

	GetRoom()->LeaveRoom(shared_from_this());
}
	
void Player::BroadCast(Asset::MsgItem& item) 
{
	if (!_locate_room) return;
	
}	

void Player::AlterError(Asset::ERROR_CODE error_code, Asset::ERROR_TYPE error_type/*= Asset::ERROR_TYPE_NORMAL*/, 
		Asset::ERROR_SHOW_TYPE error_show_type/* = Asset::ERROR_SHOW_TYPE_CHAT*/)
{
	Asset::AlterError message;
	message.set_error_type(error_type);
	message.set_error_show_type(error_show_type);
	message.set_error_code(error_code);
	SendProtocol(message);
}

/////////////////////////////////////////////////////
/////游戏逻辑定义
/////////////////////////////////////////////////////
int Player::ZhuaPai()
{
	return 0;
}

int Player::ChiPai()
{
	return 0;
}

int Player::PengPai()
{
	return 0;
}

int Player::GangPai()
{
	return 0;
}

int32_t Player::OnFaPai(std::vector<int32_t>& cards)
{

	return 0;
}

int32_t Player::OnFaPai(int32_t card)
{
	return 0;
}
/////////////////////////////////////////////////////
//玩家通用管理类
/////////////////////////////////////////////////////
void PlayerManager::Add(std::shared_ptr<Player> player)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_entities.find(player->GetID()) == _entities.end())
	{
		_entities.emplace(player->GetID(), player);
		std::cout << "Add player to manager current size is:" << _entities.size() << ", added id:" << player->GetID() << std::endl;
	}
}

void PlayerManager::Emplace(int64_t player_id, std::shared_ptr<Player> player)
{
	try
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_entities.find(player_id) == _entities.end())
			_entities.emplace(player_id, player);
		std::cout << __func__ << ":size:" << _entities.size() << ":id:" << player->GetID() << std::endl;
	}
	catch(std::exception& ex)
	{
		std::cout << __LINE__ << ex.what() << std::endl;
	}
}

std::shared_ptr<Player> PlayerManager::GetPlayer(int64_t player_id)
{
	std::lock_guard<std::mutex> lock(_mutex);
	auto it = _entities.find(player_id);
	if (it == _entities.end()) return nullptr;
	return it->second;
}
	
bool PlayerManager::Has(int64_t player_id)
{
	auto player = GetPlayer(player_id);
	return player != nullptr;
}

void PlayerManager::Erase(int64_t player_id)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_entities.erase(player_id);
}

void PlayerManager::Erase(Player& player)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_entities.erase(player.GetID());
}

}
