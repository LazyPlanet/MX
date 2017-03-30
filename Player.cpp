#include <iostream>

#include <hiredis.h>

#include "Player.h"
#include "Game.h"
#include "Timer.h"
#include "Mall.h"
#include "Player.h"
#include "Protocol.h"
#include "CommonUtil.h"
#include "RedisManager.h"
#include "PlayerCommonReward.h"
#include "PlayerCommonLimit.h"
#include "MessageFormat.h"
#include "PlayerMatch.h"

namespace Adoter
{

extern const Asset::CommonConst* g_const;

Player::Player()
{
	//协议默认处理函数
	_method = std::bind(&Player::DefaultMethod, this, std::placeholders::_1);

	//协议处理回调初始化
	AddHandler(Asset::META_TYPE_SHARE_CREATE_ROOM, std::bind(&Player::CmdCreateRoom, this, std::placeholders::_1));
	AddHandler(Asset::META_TYPE_SHARE_GAME_OPERATION, std::bind(&Player::CmdGameOperate, this, std::placeholders::_1));
	AddHandler(Asset::META_TYPE_SHARE_PAI_OPERATION, std::bind(&Player::CmdPaiOperate, this, std::placeholders::_1));
	AddHandler(Asset::META_TYPE_SHARE_BUY_SOMETHING, std::bind(&Player::CmdBuySomething, this, std::placeholders::_1));
	AddHandler(Asset::META_TYPE_SHARE_ENTER_ROOM, std::bind(&Player::CmdEnterRoom, this, std::placeholders::_1));
	AddHandler(Asset::META_TYPE_SHARE_SIGN, std::bind(&Player::CmdSign, this, std::placeholders::_1));

	//AddHandler(Asset::META_TYPE_C2S_LOGIN, std::bind(&Player::CmdLogin, this, std::placeholders::_1));
	//AddHandler(Asset::META_TYPE_C2S_ENTER_GAME, std::bind(&Player::CmdEnterGame, this, std::placeholders::_1));
	AddHandler(Asset::META_TYPE_C2S_GET_REWARD, std::bind(&Player::CmdGetReward, this, std::placeholders::_1));
	AddHandler(Asset::META_TYPE_C2S_LOAD_SCENE, std::bind(&Player::CmdLoadScene, this, std::placeholders::_1));
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
		int32_t curr_inventories_size = _stuff.inventory().inventory_size(); 
		if (curr_inventories_size == enum_desc->value_count() - 1) break; 
		for (int inventory_index = curr_inventories_size; inventory_index < enum_desc->value_count() - 1; ++inventory_index)
		{
			auto inventory = _stuff.mutable_inventory()->mutable_inventory()->Add(); //增加新包裹，且初始化数据
			inventory->set_inventory_type((Asset::INVENTORY_TYPE)(inventory_index + 1));
			//提示信息
			const pb::EnumValueDescriptor *enum_value = enum_desc->value(inventory_index);
			if (!enum_value) break;
		}
	} while(false);

	return 0;
}

int32_t Player::Save()
{
	//存入数据库
	std::string stuff = this->_stuff.SerializeAsString();

	auto redis = make_unique<Redis>();

	redis->SavePlayer(GetID(), stuff);

	return 0;
}
	
std::string Player::GetString()
{
	::google::protobuf::MessageFormat::Printer printer;
	printer.SetSingleLineMode(true); //整行打印
	printer.SetUseUtf8StringEscaping(true);

	std::string output;
	printer.PrintToString(_stuff, &output);

	return output;
}

int32_t Player::OnLogin(pb::Message* message)
{
	if (Load()) return 1;

	SendPlayer(); //发送数据给Client
	
	this->_stuff.set_login_time(CommonTimerInstance.GetTime());
	this->_stuff.set_logout_time(0);

	return 0;
}

int32_t Player::OnLogout(pb::Message* message)
{
	if (_locate_room) _locate_room->Remove(GetID()); //如果在房间里则退出

	this->_stuff.set_login_time(0);
	this->_stuff.set_logout_time(CommonTimerInstance.GetTime());
	//非存盘数据
	this->_stuff.mutable_player_prop()->Clear(); 
	//存档数据库
	Save();	
	//日志
	auto log = make_unique<Asset::LogMessage>();
	log->set_player_id(GetID());
	log->set_type(Asset::PLAYER_INFO);
	log->set_content(GetString());

	LOG(INFO, log.get()); //记录日志

	return 0;
}
	
/*
void Player::OnCreatePlayer(int64_t player_id)
{
	Asset::CreatePlayer create_player;
	create_player.set_player_id(player_id);
	SendProtocol(create_player);
}

int32_t Player::CmdEnterGame(pb::Message* message)
{
	OnEnterGame();
	return 0;
}
*/

int32_t Player::OnEnterGame() 
{
	if (Load()) return 1;

	SendPlayer(); //发送数据给玩家
	
	this->_stuff.set_login_time(CommonTimerInstance.GetTime());
	this->_stuff.set_logout_time(0);
	
	return 0;
}

int32_t Player::CmdLeaveRoom(pb::Message* message)
{
	OnLeaveRoom(); //房间处理
	return 0;
}

void Player::SendPlayer()
{
	Asset::PlayerInformation player_info;
	player_info.mutable_player()->CopyFrom(this->_stuff);

	SendProtocol(player_info);
}

int32_t Player::CmdCreateRoom(pb::Message* message)
{
	Asset::CreateRoom* create_room = dynamic_cast<Asset::CreateRoom*>(message);
	if (!create_room) return 1;

	int64_t room_id = RoomInstance.CreateRoom();
	if (!room_id) return 2;

	create_room->mutable_room()->set_room_id(room_id);
	create_room->mutable_room()->set_room_type(Asset::ROOM_TYPE_FRIEND); //创建房间，其实是好友房
	
	SendProtocol(create_room); 
	
	OnCreateRoom(create_room); //创建房间成功，直接将玩家设置到该房间

	return 0;
}

void Player::OnCreateRoom(Asset::CreateRoom* create_room)
{
	if (!create_room) return;

	Asset::Room asset_room;
	asset_room.CopyFrom(create_room->room());

	_locate_room = std::make_shared<Room>(asset_room);
	_locate_room->OnCreated();

	_locate_room->Enter(shared_from_this()); //玩家进入房间

	RoomInstance.OnCreateRoom(_locate_room); //房间管理
}

int32_t Player::CmdGameOperate(pb::Message* message)
{
	auto game_operate = dynamic_cast<Asset::GameOperation*>(message);
	if (!game_operate) return 1;
	
	if (!_locate_room) return 2; //如果玩家不在房间，也不存在后面的逻辑

	game_operate->set_source_player_id(GetID()); //设置当前操作玩家

	switch(game_operate->oper_type())
	{
		case Asset::GAME_OPER_TYPE_START: //开始游戏：相当于准备
		case Asset::GAME_OPER_TYPE_LEAVE: //离开游戏：相当于退出房间
		{
			_stuff.mutable_player_prop()->set_game_oper_state(game_operate->oper_type());
		}
		break;

		case Asset::GAME_OPER_TYPE_KICKOUT: //踢人
		{
			if (!_locate_room->IsHoster(GetID())) //不是房主，不能踢人
			{
				AlertMessage(Asset::ERROR_ROOM_NO_PERMISSION); //没有权限
				return 3;
			}
		}
		break;
		
		case Asset::GAME_OPER_TYPE_NULL: 
		default:
		{
			 _stuff.mutable_player_prop()->clear_game_oper_state();
		}
		break;
	}

	_locate_room->OnPlayerOperate(shared_from_this(), message); //广播给其他玩家

	return 0;
}

int32_t Player::CmdPaiOperate(pb::Message* message)
{
	Asset::PaiOperation* pai_operate = dynamic_cast<Asset::PaiOperation*>(message);
	if (!pai_operate) return 1; 
	
	if (!_locate_room || !_game) return 2; //还没加入房间或者还没开始游戏

	//if (pai_operate->pais().size() <= 0 && pai_operate->pai().size() <= 0) return 3; //估计是外挂

	pai_operate->set_position(GetPosition()); //设置玩家座位

	//const auto& pais = _cards[pai_operate->pai().card_type()];
	//auto it = std::find(pais.begin(), pais.end(), pai_operate->pai().card_value());

	//if (it == pais.end()) return 5; //没有这张牌

	//进行操作
	switch (pai_operate->oper_type())
	{
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_DAPAI: //打牌
		{
			const auto& pai = pai_operate->pai(); 

			auto& pais = _cards[pai.card_type()]; //获取该类型的牌

			auto it = std::find(pais.begin(), pais.end(), pai.card_value()); //查找第一个满足条件的牌即可
			if (it == pais.end()) return 6; //没有这张牌

			pais.erase(it); //打出牌
		}
		break;
		
		//其余的操作都转入到Game中处理
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_HUPAI: //胡牌
		{
			//bool ret = CheckHuPai(pai); 
			//if (!ret) return 7; //估计是挂

		}
		break;
		
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_CHIPAI: //吃牌
		{
			//检查玩家是否真的有这些牌
			for (const auto& pai : pai_operate->pais()) 
			{
				const auto& pais = _cards[pai.card_type()];

				auto it = std::find(pais.begin(), pais.end(), pai.card_value());
				if (it == pais.end()) return 3; //没有这张牌

				//if (pais[pai.card_index()] != pai.card_value()) return 4; //Server<->Client 不一致 TODO:暂时不做检查
			}
		}
		break;

		default:
		{
		}
		break;

	}

	_game->OnPaiOperate(shared_from_this(), message);

	return 0;
}
	
int32_t Player::CmdSign(pb::Message* message)
{
	Asset::Sign* sign = dynamic_cast<Asset::Sign*>(message);
	if (!sign) return 1; 

	auto curr_t = CommonTimerInstance.GetTime(); //当前时间

	auto it = std::find_if(_stuff.sign_time().rbegin(), _stuff.sign_time().rend(), [curr_t](const int32_t& time) {
			return CommonTimerInstance.IsSameDay(curr_t, time); 
	});

	if (it == _stuff.sign_time().rend()) 
	{
		_stuff.mutable_sign_time()->Add(curr_t); //记录签到时间
		sign->set_success(true); //默认失败
	}

	//发奖
	auto asset_message = AssetInstance.Get(g_const->daily_sign_id());
	if (!asset_message) return 2;

	auto asset_sign = dynamic_cast<Asset::DailySign*>(asset_message);
	if (!asset_sign) return 3;

	auto common_limit_id = asset_sign->common_limit_id();
	if (!IsCommonLimit(common_limit_id)) DeliverReward(asset_sign->common_reward_id()); //正式发奖
	AddCommonLimit(common_limit_id); //今日已领取

	SendProtocol(sign);
	return 0;
}

int32_t Player::CmdEnterRoom(pb::Message* message) 
{
	Asset::EnterRoom* enter_room = dynamic_cast<Asset::EnterRoom*>(message);
	if (!enter_room) return 1; 

	if (_locate_room) return 2; //已经在房间

	Asset::ROOM_TYPE room_type = enter_room->room().room_type();

	auto check = [this, room_type]()->Asset::ERROR_CODE {

		const auto& messages = AssetInstance.GetMessagesByType(Asset::ASSET_TYPE_ROOM);

		auto it = std::find_if(messages.begin(), messages.end(), [room_type](pb::Message* message){
			auto room_limit = dynamic_cast<Asset::RoomLimit*>(message);
			if (!room_limit) return false;

			return room_type == room_limit->room_type();
		});

		if (it == messages.end()) return Asset::ERROR_ROOM_TYPE_NOT_FOUND;
		
		auto room_limit = dynamic_cast<Asset::RoomLimit*>(*it);
		if (!room_limit) return Asset::ERROR_ROOM_TYPE_NOT_FOUND;

		int64_t beans_count = GetHuanledou();

		int32_t min_limit = room_limit->min_limit();
		if (min_limit >= 0 && beans_count < min_limit) return Asset::ERROR_ROOM_BEANS_MIN_LIMIT;

		int32_t max_limit = room_limit->max_limit();
		if (max_limit >= 0 && beans_count > max_limit) return Asset::ERROR_ROOM_BEANS_MAX_LIMIT;

		return Asset::ERROR_SUCCESS;
	};


	switch (room_type)
	{
		case Asset::ROOM_TYPE_FRIEND: //好友房
		{
			auto room_id = enter_room->room().room_id(); 

			auto locate_room = RoomInstance.Get(room_id);

			if (!locate_room) 
			{
				enter_room->set_error_code(Asset::ERROR_ROOM_NOT_FOUNT); //是否可以进入场景//房间
			}
			else
			{
				auto ret = locate_room->TryEnter(shared_from_this()); //玩家进入房间
				enter_room->set_error_code(ret); //是否可以进入场景//房间
			}

			SendProtocol(enter_room);
			return 0;
		}
		break;

		case Asset::ROOM_TYPE_XINSHOU:
		case Asset::ROOM_TYPE_GAOSHOU:
		case Asset::ROOM_TYPE_DASHI:
		{
			auto result = check();

			if (result != Asset::ERROR_SUCCESS) //不允许进入
			{
				AlertMessage(result);
				return result;
			}

			//进入匹配
			MatchInstance.Join(shared_from_this(), message);
		}
		break;

		default:
		{
			return 1; //非法
		}
		break;
	}

	/*
	Asset::Room asset_room;
	asset_room.set_room_type(room_type);

	Room room(asset_room);
	room.OnCreated();

	OnCreateRoom(create_room);

	int64_t room_id = enter_room->room_id();
	if (room_id == 0) //随机进入
	{
		auto room = RoomInstance.GetAvailableRoom();
		if (!room) 
		{
			AlertMessage(Asset::ERROR_ROOM_NOT_AVAILABLE);
			return 5; //没有合适的房间
		}

		room_id = room->GetID();
	}
	else
	{
		auto room = RoomInstance.Get(room_id);
		if (!room) 
		{
			AlertMessage(Asset::ERROR_ROOM_NOT_FOUNT);
			return 2; 
		}

		if (room->IsFull()) 
		{
			AlertMessage(Asset::ERROR_ROOM_FULL);
			return 3;
		}
	}
	OnEnterRoom(room_id); //通知同房间玩家，同时在房间中初始化该玩家
	*/
	return 0;
}

bool Player::OnEnterRoom(int64_t room_id)
{
	_locate_room = RoomInstance.Get(room_id);

	if (!_locate_room) return false; //非法的房间 

	_locate_room->OnCreated();

	_locate_room->Enter(shared_from_this()); //玩家进入房间

	return true;
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

void Player::SendProtocol(pb::Message* message)
{
	SendProtocol(*message);
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
	GetSession()->AsyncSend(content);
	
	
	auto log = make_unique<Asset::LogMessage>();
	log->set_player_id(GetID());
	log->set_type(Asset::SEND_PROTOCOL);
	log->set_content(message.ShortDebugString());

	LOG(INFO, log.get()); //记录日志
}

/*
void Player::SendResponse(pb::Message* message)
{
	if (!message) return;

	const pb::FieldDescriptor* field = message->GetDescriptor()->FindFieldByName("type_t");
	if (!field) return;
	
	int type_t = field->default_value_enum()->number();
	if (!Asset::META_TYPE_IsValid(type_t)) return;	//如果不合法，不检查会宕线

	std::cout << __func__ << ":type_t:" << type_t << std::endl;
	
	Asset::CommonOperationResponse response;
	response.set_client_type_t((Asset::META_TYPE)type_t);
	response.set_client_message(message->SerializeAsString());
	SendProtocol(response);
}

void Player::SendToRoomers(pb::Message& message) 
{
}
*/
//玩家心跳周期为10MS，如果该函数返回FALSE则表示掉线
bool Player::Update()
{
	++_heart_count; //心跳

	if (_heart_count % 6000 == 0) //1MIN
	{
		std::cout << "===================Time has gone 1min, i am id:" << GetID() << std::endl;
		CommonLimitUpdate(); //通用限制,定时更新
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

bool Player::GainItem(int64_t global_item_id, int32_t count)
{
	pb::Message* asset_item = AssetInstance.Get(global_item_id); //此处取出来的必然为合法ITEM.
	if (!asset_item) return false;

	Item* item = new Item(asset_item);
	GainItem(item);
	return true;
}

bool Player::GainItem(Item* item, int32_t count)
{
	if (!item || count <= 0) return false;

	Asset::Item_CommonProp& common_prop = item->GetCommonProp(); 
	common_prop.set_count(count);

	if (!PushBackItem(common_prop.inventory(), item)) return false;
	return true;
}
	
bool Player::PushBackItem(Asset::INVENTORY_TYPE inventory_type, Item* item)
{
	if (!item) return false;

	const pb::EnumDescriptor* enum_desc = Asset::INVENTORY_TYPE_descriptor();
	if (!enum_desc) return false;

	Asset::Inventory_Element* inventory = _stuff.mutable_inventory()->mutable_inventory(inventory_type); 
	if (!inventory) return false;

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

	//清理状态
	_stuff.mutable_player_prop()->clear_game_oper_state();
	//通知房间
	//GetRoom()->LeaveRoom(shared_from_this());
}
	
void Player::BroadCast(Asset::MsgItem& item) 
{
	if (!_locate_room) return;
	
}	

void Player::AlertMessage(Asset::ERROR_CODE error_code, Asset::ERROR_TYPE error_type/*= Asset::ERROR_TYPE_NORMAL*/, 
		Asset::ERROR_SHOW_TYPE error_show_type/* = Asset::ERROR_SHOW_TYPE_CHAT*/)
{
	Asset::AlertMessage message;
	message.set_error_type(error_type);
	message.set_error_show_type(error_show_type);
	message.set_error_code(error_code);
	SendProtocol(message);
}

bool Player::AddCommonLimit(int64_t global_id)
{
	return CommonLimitInstance.AddCommonLimit(shared_from_this(), global_id);
}
	
bool Player::IsCommonLimit(int64_t global_id)
{
	return CommonLimitInstance.IsCommonLimit(shared_from_this(), global_id);
}

bool Player::CommonLimitUpdate()
{
	bool updated = CommonLimitInstance.Update(shared_from_this());
	if (updated) SyncCommonLimit();

	return updated;
}

void Player::SyncCommonLimit()
{
	Asset::SyncCommonLimit proto;
	proto.mutable_common_limit()->CopyFrom(_stuff.common_limit());

	SendProtocol(proto);
}

bool Player::DeliverReward(int64_t global_id)
{
	bool delivered = CommonRewardInstance.DeliverReward(shared_from_this(), global_id);
	if (delivered) SyncCommonReward(global_id);
	
	return delivered;
}

void Player::SyncCommonReward(int64_t common_reward_id)
{
	Asset::SyncCommonReward proto;
	proto.set_common_reward_id(common_reward_id);

	SendProtocol(proto);
}

int32_t Player::CmdGetReward(pb::Message* message)
{
	Asset::GetReward* get_reward = dynamic_cast<Asset::GetReward*>(message);
	if (!get_reward) return 1;

	int64_t reward_id = get_reward->reward_id();
	if (reward_id <= 0) return 2;

	switch (get_reward->reason())
	{
		case Asset::GetReward_GET_REWARD_REASON_GET_REWARD_REASON_DAILY_BONUS: //每日登陆奖励
		{
			int64_t daily_bonus_id = g_const->daily_bonus_id();
			if (reward_id != daily_bonus_id) return 3; //Client和Server看到的数据不一致
			
			auto message = AssetInstance.Get(daily_bonus_id);
			if (!message) return 4;

			auto bonus = dynamic_cast<Asset::DailyBonus*>(message);
			if (!bonus) return 5;

			int64_t common_limit_id = bonus->common_limit_id();
			if (IsCommonLimit(common_limit_id)) 
			{
				AlertMessage(Asset::ERROR_REWARD_HAS_GOT);
				return 6;
			}
			
			bool ret = DeliverReward(bonus->common_reward_id()); //发奖
			if (!ret) return 7;
		
			AddCommonLimit(common_limit_id);
		}
		break;
		
		case Asset::GetReward_GET_REWARD_REASON_GET_REWARD_REASON_DAILY_ALLOWANCE: //每日补助奖励
		{
			int64_t daily_allowance_id = g_const->daily_allowance_id();
			if (reward_id != daily_allowance_id) return 3; //Client和Server看到的数据不一致
			
			auto message = AssetInstance.Get(daily_allowance_id);
			if (!message) return 4;

			auto allowance = dynamic_cast<Asset::DailyAllowance*>(message);
			if (!allowance) return 5;

			int32_t huanledou_below = allowance->huanledou_below(); 
			if (huanledou_below > 0 && huanledou_below < GetHuanledou())
			{
				AlertMessage(Asset::ERROR_HUANLEDOU_LIMIT); //欢乐豆数量不满足
				return 8;
			}

			int64_t common_limit_id = allowance->common_limit_id();
			if (IsCommonLimit(common_limit_id)) 
			{
				AlertMessage(Asset::ERROR_REWARD_HAS_GOT);
				return 6;
			}
			
			bool ret = DeliverReward(allowance->common_reward_id()); //发奖
			if (!ret) return 7;
		
			AddCommonLimit(common_limit_id);
		}
		break;

		default:
		{

		}
		break;
	}

	return 0;
}

void Player::SyncCommonProperty()
{
	Asset::CommonProperty common_prop;
	common_prop.set_reason_type(Asset::CommonProperty_SYNC_REASON_TYPE_SYNC_REASON_TYPE_SELF);
	common_prop.set_player_id(GetID());
	common_prop.mutable_common_prop()->CopyFrom(GetCommonProp());
}

bool Player::CmdBuySomething(pb::Message* message)
{
	auto some_thing = dynamic_cast<Asset::BuySomething*>(message);
	if (some_thing) return false;

	int64_t mall_id = some_thing->mall_id();
	if (mall_id <= 0) return false;

	auto ret = MallInstance.BuySomething(shared_from_this(), mall_id);
	some_thing->set_result(ret);

	SendProtocol(some_thing); //返回给Client

	return true;
}

int32_t Player::CmdLoadScene(pb::Message* message)
{
	Asset::LoadScene* load_scene = dynamic_cast<Asset::LoadScene*>(message);
	if (!load_scene) return 1;

	switch (load_scene->load_type())
	{
		case Asset::LOAD_SCENE_TYPE_START: //加载开始
		{
			_stuff.mutable_player_prop()->set_load_type(Asset::LOAD_SCENE_TYPE_START);
			
			_stuff.mutable_player_prop()->set_room_id(load_scene->scene_id()); //状态
		}
		break;
		
		case Asset::LOAD_SCENE_TYPE_SUCCESS: //加载成功
		{
			if (_stuff.player_prop().load_type() != Asset::LOAD_SCENE_TYPE_START) return 2;

			SendPlayer(); //发送数据给客户端

			auto room_id = _stuff.player_prop().room_id();
			
			auto locate_room = RoomInstance.Get(room_id);
			if (!locate_room) return 3; //非法的房间 
			
			auto ret = locate_room->TryEnter(shared_from_this()); //玩家进入房间

			if (ret != Asset::ERROR_SUCCESS) return 4;

			_locate_room = locate_room;

			_locate_room->Enter(shared_from_this()); //玩家进入房间
			
			_stuff.mutable_player_prop()->clear_load_type(); //状态
			_stuff.mutable_player_prop()->clear_room_id(); //状态
		}
		break;
		
		default:
		{

		}
		break;
	}

	return 0;
}

void Player::OnEnterScene()
{
	SendPlayer(); //发送数据给客户端
}

int32_t Player::CmdLuckyPlate(pb::Message* message)
{
	auto lucky_plate = dynamic_cast<Asset::PlayerLuckyPlate*>(message);
	if (!lucky_plate) return 1;

	auto asset_message = AssetInstance.Get(lucky_plate->plate_id());
	if (!asset_message) return 2;
	
	auto asset_lucky_plate = dynamic_cast<Asset::LuckyPlate*>(asset_message);
	if (!asset_lucky_plate) return 3;

	auto index = CommonUtil::RandomByWeight(asset_lucky_plate->plates().begin(), asset_lucky_plate->plates().end(), 
			[](const Asset::LuckyPlate_Plate& ele){
				return ele.weight();
			});

	if (index < 0 || index >= asset_lucky_plate->plates().size() || index >= asset_lucky_plate->plates_count()) return 4;

	auto common_reward_id = asset_lucky_plate->plates(index).common_reward_id();
	DeliverReward(common_reward_id); //发奖

	lucky_plate->set_result(index + 1); //Client从1开始
	SendProtocol(lucky_plate);

	return 0;
}

/////////////////////////////////////////////////////
/////游戏逻辑定义
/////////////////////////////////////////////////////
std::vector<Asset::PAI_CHECK_RETURN> Player::CheckPai(const Asset::PaiElement& pai)
{
	std::cout << "玩家来的牌:line:" << __LINE__ << "card_type:" << pai.card_type() << " card_value:" << pai.card_value() << std::endl;

	PrintPai();

	std::vector<Asset::PAI_CHECK_RETURN> rtn_check;

	if (CheckHuPai(pai)) 
	{
		std::cout << "玩家胡牌line:" << __LINE__ << std::endl;
		rtn_check.push_back(Asset::PAI_CHECK_RETURN_HU);
	}
	if (CheckGangPai(pai)) 
	{
		std::cout << "玩家杠牌line:" << __LINE__ << std::endl;
		rtn_check.push_back(Asset::PAI_CHECK_RETURN_GANG);
	}
	if (CheckPengPai(pai)) 
	{
		std::cout << "玩家碰牌line:" << __LINE__ << std::endl;
		rtn_check.push_back(Asset::PAI_CHECK_RETURN_PENG);
	}
	if (CheckChiPai(pai)) 
	{
		std::cout << "玩家吃牌line:" << __LINE__ << std::endl;
		rtn_check.push_back(Asset::PAI_CHECK_RETURN_CHI);
	}
		
	return rtn_check;
}

//假定牌是排序过的, 且胡牌规则为 n*AAA+m*ABC+DD
//
//用 A + 点数 表示 万子(A1 表示 1万, 依此类推)
//用 B + 点数 表示 筒子(B1 表示 1筒, 依此类推)
//用 C + 点数 表示 索子(C1 表示 1索, 依此类推)
//
//字只有东西南北中发白:假定用D1-D7表示吧.
//
//算法逻辑: 首张牌用于对子，顺子, 或者三张.
//接下来递归判断剩下牌型是否能和, 注意对子只能用一次.
//
//下面的算法是可以直接判断是否牌型是否和牌的，不局限于14张牌(3n+2即可)

struct Card_t {

	int32_t _type; //类型
	int32_t _value; //值

public:
	bool operator == (const Card_t& card)
	{
		return _type == card._type && _value == card._value;
	}

	Card_t operator + (int32_t value)
	{
		return Card_t(_type, _value + value);
	}

	Card_t(int32_t type, int32_t value)
	{
		_type = type;
		_value = value;
	}
};

bool CanHuPai(std::vector<Card_t>& cards, bool use_pair = false)
{
	int32_t size = cards.size();

	if (size <= 2) 
	{
		return size == 0 || cards[0] == cards[1]; 
	}

	bool pair = false/*一对*/, straight/*顺子//一套副*/ = false;

	if (!use_pair)
	{
		std::vector<Card_t> sub_cards(cards.begin() + 2, cards.end());

		pair = (cards[0] == cards[1]) && CanHuPai(sub_cards, true);
	}

	//这里有个判断, 如果只剩两张牌而又不是对子肯定不算和牌,跳出是防止下面数组越界。
	//
	//首张牌用以三张, 剩下的牌是否能和牌。
	
	std::vector<Card_t> sub_cards(cards.begin() + 3, cards.end());
	bool trips = (cards[0] == cards[1]) && (cards[1] == cards[2]) && CanHuPai(sub_cards, use_pair); //刻:三个一样的牌

	int32_t card_value = cards[0]._value;
	if (card_value <= 7)
	{
		//顺子的第一张牌
		auto first = cards[0];
		//顺子的第二张牌
		auto second = cards[0] + 1;
		//顺子的第三张牌
		auto third = cards[0] + 2;
		//玩家是否真的有这两张牌
		if (std::find(cards.begin(), cards.end(), second) != cards.end() && std::find(cards.begin(), cards.end(), third) != cards.end())
		{
			//去掉用以顺子的三张牌后是否能和牌
			auto it_first = std::find(cards.begin(), cards.end(), first);
			cards.erase(it_first); //删除
			auto it_second = std::find(cards.begin(), cards.end(), second); //由于前面已经删除了元素，索引已经发生了变化，需要重新查找
			cards.erase(it_second); //删除
			auto it_third = std::find(cards.begin(), cards.end(), third); //由于前面已经删除了元素，索引已经发生了变化，需要重新查找
			cards.erase(it_third); //删除

			//顺子
			straight = CanHuPai(cards, use_pair);
		}
	}

	return pair || trips || straight; //一对、刻或者顺子
}

bool Player::CheckHuPai(const Asset::PaiElement& pai)
{
	auto cards = _cards; //复制当前牌
	cards[pai.card_type()].push_back(pai.card_value());
	
	int32_t count_sum = 0;
	//每个玩家有14张牌，需要满足 [3 3 3 3 2] 模型才能胡牌
	for (auto crds : cards)
	{
		int32_t count = crds.second.size();
		count_sum += count;
	}
	//if (count_sum != 14) return false; //TODO: 只需要记录手中的牌即可
	
	std::sort(cards[pai.card_type()].begin(), cards[pai.card_type()].end(), [](int x, int y){ return x < y; }); //由小到大，排序

	std::vector<Card_t> card_list;

	for (auto crds : cards) //不同牌类别的牌
	{
		for (auto value : crds.second)
			card_list.push_back(Card_t(crds.first, value));
	}
		
	bool can_hu = CanHuPai(card_list);	

	return can_hu;
}

bool Player::CheckChiPai(const Asset::PaiElement& pai)
{
	auto it = _cards.find(pai.card_type());
	if (it == _cards.end()) return false;

	if (pai.card_type() != Asset::CARD_TYPE_WANZI && pai.card_type() != Asset::CARD_TYPE_BINGZI &&
			pai.card_type() != Asset::CARD_TYPE_TIAOZI) return false; //万子牌、饼子牌和条子牌才能吃牌

	int32_t card_value = pai.card_value();

	//吃牌总共有有三种方式:
	//
	//比如上家出4万，可以吃的条件是：2/3; 5/6; 3/5 三种方法.
	
	if (std::find(it->second.begin(), it->second.end(), card_value - 1) != it->second.end() && 
		std::find(it->second.begin(), it->second.end(), card_value - 2) != it->second.end()) return true; 
	
	if (std::find(it->second.begin(), it->second.end(), card_value + 1) != it->second.end() && 
		std::find(it->second.begin(), it->second.end(), card_value + 2) != it->second.end()) return true; 

	if (std::find(it->second.begin(), it->second.end(), card_value - 1) != it->second.end() && 
		std::find(it->second.begin(), it->second.end(), card_value + 1) != it->second.end()) return true; 

	return false;
}

void Player::OnChiPai(const Asset::PaiElement& pai, pb::Message* message)
{
	if (!CheckChiPai(pai) || !message) return;

	Asset::PaiOperation* pai_operate = dynamic_cast<Asset::PaiOperation*>(message);
	if (!pai_operate) return;
	
	std::vector<Asset::PaiElement> cards;

	cards.push_back(pai);

	if (pai_operate->pais().size() != 2) return; 
	
	auto it = _cards.find(pai.card_type());
	if (it == _cards.end()) return;
		
	for (const auto& p : pai_operate->pais())	
	{
		if (pai.card_type() != p.card_type()) return; //牌类型不一致

		cards.push_back(p);
	}

	std::sort(cards.begin(), cards.end(), [](const Asset::PaiElement& rv, const Asset::PaiElement& lv) {
			return rv.card_value() < lv.card_value();
		});

	if (cards[1].card_value() - cards[0].card_value() != 1 || 
			cards[2].card_value() - cards[1].card_value() != 1) return; //不是顺子

	auto first = std::find(it->second.begin(), it->second.end(), pai_operate->pais(0).card_value());
	if (first == it->second.end()) return; //理论上不会出现
	
	it->second.erase(first); //删除
	
	auto second = std::find(it->second.begin(), it->second.end(), pai_operate->pais(1).card_value());
	if (second == it->second.end()) return; //理论上不会出现

	it->second.erase(second); //删除

	SynchronizePai();
}

bool Player::CheckPengPai(const Asset::PaiElement& pai)
{
	auto it = _cards.find(pai.card_type());
	if (it == _cards.end()) return false;

	int32_t card_value = pai.card_value();
	int32_t count = std::count_if(it->second.begin(), it->second.end(), [card_value](int32_t value) { return card_value == value; });

	if (count < 2) return false;
	
	return true;
}

void Player::OnPengPai(const Asset::PaiElement& pai)
{
	if (!CheckPengPai(pai)) return;
	
	auto it = _cards.find(pai.card_type());
	if (it == _cards.end()) return; //理论上不会如此
	
	int32_t card_value = pai.card_value();
	std::remove(it->second.begin(), it->second.end(), card_value); //从玩家手里删除，TODO:玩家手里有3张牌，但是选择了碰牌...
	
	SynchronizePai();
}

bool Player::CheckGangPai(const Asset::PaiElement& pai)
{
	auto it = _cards.find(pai.card_type());
	if (it == _cards.end()) return false;

	int32_t card_value = pai.card_value();
	int32_t count = std::count_if(it->second.begin(), it->second.end(), [card_value](int32_t value) { return card_value == value; });

	if (3 > count) return false; //如果玩家有3张，则是明杠，如果玩家有4张则是暗杠
	
	return true;
}
	
void Player::OnGangPai(const Asset::PaiElement& pai)
{
	if (!CheckGangPai(pai)) return;

	auto it = _cards.find(pai.card_type());
	if (it == _cards.end()) return; //理论上不会如此
	
	int32_t card_value = pai.card_value();
	std::remove(it->second.begin(), it->second.end(), card_value); //从玩家手里删除
	
	SynchronizePai();
}

bool Player::CheckAnGangPai(Asset::PaiElement& pai)
{
	for (auto card : _cards)
	{
		for (auto card_value : card.second)
		{
			int32_t count = std::count_if(card.second.begin(), card.second.end(), [card_value](int32_t value) { return card_value == value; });

			if (count == 4) //暗杠
			{
				pai.set_card_type((Asset::CARD_TYPE)card.first);
				pai.set_card_value(pai.card_value());
				return true;
			}
		}
	}
	
	return false;
}

bool Player::CheckFengGangPai()
{
	auto it = _cards.find(Asset::CARD_TYPE_FENG);
	for (auto card_value = 1; card_value <= 4; ++card_value) //东南西北
	{
		auto it_if = std::find(it->second.begin(), it->second.end(), card_value);
		if (it_if == it->second.end()) return false;
	}
	return true;
}

void Player::OnGangFengPai()
{
	if (!CheckFengGangPai()) return;

	auto it = _cards.find(Asset::CARD_TYPE_FENG);
	for (auto card_value = 1; card_value <= 4; ++card_value) //东南西北
	{
		auto it_if = std::find(it->second.begin(), it->second.end(), card_value);
		if (it_if != it->second.end())  it->second.erase(it_if); //删除
	}
}

bool Player::CheckJianGangPai()
{
	auto it = _cards.find(Asset::CARD_TYPE_JIAN);
	for (auto card_value = 1; card_value <= 3; ++card_value) //中发白
	{
		auto it_if = std::find(it->second.begin(), it->second.end(), card_value);
		if (it_if == it->second.end()) return false;
	}
	return true;
}

void Player::OnGangJianPai()
{
	if (!CheckJianGangPai()) return;

	auto it = _cards.find(Asset::CARD_TYPE_JIAN);
	for (auto card_value = 1; card_value <= 3; ++card_value) //中发白
	{
		auto it_if = std::find(it->second.begin(), it->second.end(), card_value);
		if (it_if != it->second.end())  it->second.erase(it_if); //删除
	}
}

int32_t Player::OnFaPai(std::vector<int32_t> cards)
{
	for (auto card_index : cards)
	{
		auto card = GameInstance.GetCard(card_index);
		if (card.card_type() == 0 || card.card_value() == 0) return 1; //数据有误

		_cards[card.card_type()].push_back(card.card_value());
	}

	for (auto& cards : _cards) //整理牌
	{
		std::sort(cards.second.begin(), cards.second.end(), [](int x, int y){ return x < y; }); //由小到大
	}

	PrintPai();
	
	Asset::PaiNotify notify; /////玩家当前牌数据发给Client

	if (cards.size() > 1)
	{
		for (auto pai : _cards)
		{
			auto pais = notify.mutable_pais()->Add();

			pais->set_card_type((Asset::CARD_TYPE)pai.first); //牌类型

			::google::protobuf::RepeatedField<int32_t> cards(pai.second.begin(), pai.second.end());
			pais->mutable_cards()->CopyFrom(cards); //牌值
		}
		
		notify.set_data_type(Asset::PaiNotify_CARDS_DATA_TYPE_CARDS_DATA_TYPE_START); //操作类型：开局
	}
	else if (cards.size() == 1)
	{
		auto card = GameInstance.GetCard(cards[0]);

		notify.mutable_pai()->set_card_type(card.card_type());
		notify.mutable_pai()->set_card_value(card.card_value());

		notify.set_data_type(Asset::PaiNotify_CARDS_DATA_TYPE_CARDS_DATA_TYPE_FAPAI); //操作类型：发牌

		//检查玩家手中牌是否可操作
		Asset::PaiOperationAlert alert;
		alert.mutable_pai()->CopyFrom(card);
		if (CheckHuPai(card)) alert.mutable_check_return()->Add(Asset::PAI_CHECK_RETURN_HU); 
		if (CheckGangPai(card)) alert.mutable_check_return()->Add(Asset::PAI_CHECK_RETURN_GANG); //可操作牌类型
		if (alert.check_return().size()) SendProtocol(alert);
	}
	
	SendProtocol(notify); //发送

	return 0;
}
	
void Player::SynchronizePai()
{
	Asset::PaiNotify notify; /////玩家当前牌数据发给Client

	for (auto pai : _cards)
	{
		auto pais = notify.mutable_pais()->Add();

		pais->set_card_type((Asset::CARD_TYPE)pai.first); //牌类型

		::google::protobuf::RepeatedField<int32_t> cards(pai.second.begin(), pai.second.end());
		pais->mutable_cards()->CopyFrom(cards); //牌值
	}
	
	notify.set_data_type(Asset::PaiNotify_CARDS_DATA_TYPE_CARDS_DATA_TYPE_SYNC); //操作类型：同步数据
	
	SendProtocol(notify); //发送
}

void Player::PrintPai()
{
	Asset::PaiNotify notify; /////玩家当前牌数据发给Client

	for (auto pai : _cards)
	{
		auto pais = notify.mutable_pais()->Add();

		pais->set_card_type((Asset::CARD_TYPE)pai.first); //牌类型

		::google::protobuf::RepeatedField<int32_t> cards(pai.second.begin(), pai.second.end());
		pais->mutable_cards()->CopyFrom(cards); //牌值
	}
	
	notify.set_data_type(Asset::PaiNotify_CARDS_DATA_TYPE_CARDS_DATA_TYPE_SYNC); //操作类型：同步数据
	
	std::cout << "当前玩家牌:" << GetID() << std::endl;
	notify.PrintDebugString();
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
