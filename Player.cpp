#include <iostream>

#include <hiredis.h>

#include <spdlog/spdlog.h>
#include <pbjson.hpp>

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

namespace spd = spdlog;
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
	AddHandler(Asset::META_TYPE_SHARE_RANDOM_SAIZI, std::bind(&Player::CmdSaizi, this, std::placeholders::_1));

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
	/*
	if (Load()) return 1;

	SendPlayer(); //发送数据给Client
	
	this->_stuff.set_login_time(CommonTimerInstance.GetTime());
	this->_stuff.set_logout_time(0);
	*/
	return 0;
}

int32_t Player::OnLogout(pb::Message* message)
{
	if (_locate_room) 
	{
		Asset::GameOperation game_operate;
		game_operate.set_source_player_id(GetID()); //设置当前操作玩家
		game_operate.set_oper_type(Asset::GAME_OPER_TYPE_LEAVE); //离开游戏，退出房间
		_locate_room->OnPlayerOperate(shared_from_this(), &game_operate); //广播给其他玩家
	}

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

	//BI日志
	std::string json;
	pbjson::pb2json(&_stuff, json);
	spdlog::get("player")->info(json);

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
	if (!create_room) return; //理论不会如此

	Asset::Room asset_room;
	asset_room.CopyFrom(create_room->room());

	_locate_room = std::make_shared<Room>(asset_room);
	_locate_room->OnCreated();

	//_locate_room->Enter(shared_from_this()); //玩家进入房间

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

	if (!pai_operate->position()) pai_operate->set_position(GetPosition()); //设置玩家座位

	//进行操作
	switch (pai_operate->oper_type())
	{
		case Asset::PAI_OPER_TYPE_DAPAI: //打牌
		{
			const auto& pai = pai_operate->pai(); 

			auto& pais = _cards[pai.card_type()]; //获取该类型的牌

			auto it = std::find(pais.begin(), pais.end(), pai.card_value()); //查找第一个满足条件的牌即可
			
			if (it == pais.end()) 
			{
				DEBUG_ASSERT(false);
				return 6; //没有这张牌
			}

			DEBUG("%s:line:%d,玩家:%ld删除手中的牌, 类型:%d, 值:%d\n", __func__, __LINE__, GetID(), pai.card_type(), pai.card_value());

			pais.erase(it); //打出牌
		}
		break;
		
		case Asset::PAI_OPER_TYPE_CHIPAI: //吃牌
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
		
		case Asset::PAI_OPER_TYPE_XUANFENG_FENG: //旋风杠
		{
			if (_stuff.player_prop().pai_oper_count() >= 2) 
			{
				P(Asset::ERROR, "%s:line:%d, player:%ld 检查旋风杠，估计外挂行为.", __func__, __LINE__, GetID());
				return 4;
			}
			OnGangFengPai();
		}
		break;
		
		case Asset::PAI_OPER_TYPE_XUANFENG_JIAN: //旋风杠
		{
			if (_stuff.player_prop().pai_oper_count() >= 2) 
			{
				P(Asset::ERROR, "%s:line:%d, player:%ld 检查旋风杠，估计外挂行为.", __func__, __LINE__, GetID());
				return 4;
			}
			OnGangJianPai();
		}
		break;
		
		case Asset::PAI_OPER_TYPE_TINGPAI: //听牌
		{
			if (!CheckTingPai()) return 5;

			_stuff.mutable_player_prop()->set_has_tinged(true);
			_game->AddTingPlayer(GetID());
		}
		break;

		default:
		{
		}
		break;

	}

	_game->OnPaiOperate(shared_from_this(), message);

	_stuff.mutable_player_prop()->set_pai_oper_count(_stuff.player_prop().pai_oper_count() + 1); //玩家操作次数

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
	if (!IsConnect()) 
	{
		spdlog::get("player")->error("{0}:Line:{1} player_id:{2} has been offline.", __func__, __LINE__, GetID());
		DEBUG_ASSERT(false);
	}

	//DEBUG("%s:line:%d player_id:%ld\n", __func__, __LINE__, GetID());
	GetSession()->SendProtocol(message);
	/*
	const pb::FieldDescriptor* field = message.GetDescriptor()->FindFieldByName("type_t");
	if (!field) return;
	
	int type_t = field->default_value_enum()->number();
	if (!Asset::META_TYPE_IsValid(type_t)) return;	//如果不合法，不检查会宕线
	
	Asset::Meta meta;
	meta.set_type_t((Asset::META_TYPE)type_t);
	meta.set_stuff(message.SerializeAsString());

	std::string content = meta.SerializeAsString();

	GetSession()->EnterQueue(std::move(content));
	//GetSession()->AsyncSend(content);
	
	const pb::EnumValueDescriptor* enum_value = message.GetReflection()->GetEnum(message, field);
	if (!enum_value) return;
	DEBUG("%s:line:%d, protocol_name:%s, content:%s\n", __func__, __LINE__, enum_value->name().c_str(), message.ShortDebugString().c_str());
	*/
	/*
	auto log = make_unique<Asset::LogMessage>();
	log->set_player_id(GetID());
	log->set_type(Asset::SEND_PROTOCOL);
	log->set_content(message.ShortDebugString());

	LOG(INFO, log.get()); //记录日志
	*/
}

/*
void Player::SendToRoomers(pb::Message& message) 
{
}
*/
//玩家心跳周期为10MS，如果该函数返回FALSE则表示掉线
bool Player::Update()
{
	++_heart_count; //心跳
	
	if (_heart_count % 1000 == 0) //10s
	{
		if (_dirty) Save(); //触发存盘
		CommonLimitUpdate(); //通用限制,定时更新
	}

	if (_heart_count % 6000 == 0) //1min
	{
		spdlog::get("console")->debug("{0} Line:{1} heart_count:{2} player_id:{3}", __func__, __LINE__, _heart_count, GetID());
	}
	return true;
}
	
int32_t Player::DefaultMethod(pb::Message* message)
{
	if (!message) return 0;

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

int32_t Player::CmdSaizi(pb::Message* message)
{
	auto saizi = dynamic_cast<Asset::RandomSaizi*>(message);
	if (!saizi) return 1;

	int32_t result = CommonUtil::Random(1, 6);
	saizi->set_random_result(result);

	SendProtocol(saizi);
	return 0;
}

/////////////////////////////////////////////////////
/////游戏逻辑定义
/////////////////////////////////////////////////////
std::vector<Asset::PAI_OPER_TYPE> Player::CheckPai(const Asset::PaiElement& pai, int64_t from_player_id)
{
	spdlog::get("console")->debug("{0} Line:{1} player_id:{2} card_type:{3} card_value:{4}", 
			__func__, __LINE__, GetID(), pai.card_type(), pai.card_value());

	PrintPai();

	std::vector<Asset::PAI_OPER_TYPE> rtn_check;
	std::vector<Asset::FAN_TYPE> fan_list;

	if (CheckHuPai(pai, fan_list)) 
	{
		std::cout << "玩家胡牌line:" << __LINE__ << std::endl;
		rtn_check.push_back(Asset::PAI_OPER_TYPE_HUPAI);
	}
	if (CheckGangPai(pai, from_player_id)) 
	{
		std::cout << "玩家杠牌line:" << __LINE__ << std::endl;
		rtn_check.push_back(Asset::PAI_OPER_TYPE_GANGPAI);
	}
	if (CheckPengPai(pai)) 
	{
		std::cout << "玩家碰牌line:" << __LINE__ << std::endl;
		rtn_check.push_back(Asset::PAI_OPER_TYPE_PENGPAI);
	}
	if (CheckChiPai(pai)) 
	{
		std::cout << "玩家吃牌line:" << __LINE__ << std::endl;
		rtn_check.push_back(Asset::PAI_OPER_TYPE_CHIPAI);
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

std::vector<std::tuple<bool, bool, bool>> hu_result;

bool CanHuPai(std::vector<Card_t>& cards, bool use_pair = false)
{
	int32_t size = cards.size();

	if (size <= 2) 
	{
		if (size == 1)	P(Asset::ERROR, "%s:line:%d, size=1.", __func__, __LINE__);

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

	int32_t card_value = cards[0]._value, card_type = cards[0]._type;

	if (card_value <= 7 && card_type != Asset::CARD_TYPE_FENG && card_type != Asset::CARD_TYPE_JIAN)
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

	hu_result.push_back(std::make_tuple(pair, trips, straight));

	return pair || trips || straight; //一对、刻或者顺子
}
	
bool Player::CheckBaoHu(const Asset::PaiElement& pai)
{
	if (!_game || !_locate_room) return false;

	if (!IsTingPai()) return false; //没有听牌显然不能胡宝牌

	auto baopai = _game->GetBaoPai();

	if (pai.card_type() != baopai.card_type() || pai.card_value() != baopai.card_value())  return false; //不是宝牌
	
	auto options = _locate_room->GetOptions();
	
	auto it_baohu = std::find(options.extend_type().begin(), options.extend_type().end(), Asset::ROOM_EXTEND_TYPE_BAOPAI);
	if (it_baohu == options.extend_type().end()) return false; //不带宝胡

	return true;
}

bool Player::CheckHuPai(const Asset::PaiElement& pai, std::vector<Asset::FAN_TYPE>& fan_list)
{
	spdlog::get("console")->debug("{0} Line:{1} player_id:{2} card_type:{3} card_value:{4}", 
			__func__, __LINE__, GetID(), pai.card_type(), pai.card_value());

	auto cards = _cards; //复制当前牌

	for (auto crds : _cards_outhand) //复制牌外牌
		cards[crds.first].insert(cards[crds.first].end(), crds.second.begin(), crds.second.end());

	cards[pai.card_type()].push_back(pai.card_value()); //放入可以操作的牌
	
	for (auto& card : cards)
		std::sort(card.second.begin(), card.second.end(), [](int x, int y){ return x < y; }); //由小到大，排序

	bool zhanlihu = false, jiahu = false, xuanfenggang = false, baopai = false, duanmen = false, yise = false, piao = false, baohu = false; //积分

	////////////////////////////////////////////////////////////////////////////是否可以胡牌的前置检查
	auto options = _locate_room->GetOptions();

	////////是否可以缺门、清一色
	{
		int32_t has_count = 0; //万饼条数量

		auto gang_list = _minggang;
		gang_list.insert(gang_list.end(), _angang.begin(), _angang.end());

		//是否含有万子
		auto it_wanzi = std::find_if(gang_list.begin(), gang_list.end(), [](const Asset::PaiElement& pai){
					return pai.card_type() == Asset::CARD_TYPE_WANZI;
				});
		if (cards[Asset::CARD_TYPE_WANZI].size() > 0 || it_wanzi != gang_list.end()) ++has_count;

		//是否含有饼子
		auto it_bingzi = std::find_if(gang_list.begin(), gang_list.end(), [](const Asset::PaiElement& pai){
					return pai.card_type() == Asset::CARD_TYPE_BINGZI;
				});
		if (cards[Asset::CARD_TYPE_BINGZI].size() > 0 || it_bingzi != gang_list.end()) ++has_count; 
		
		//是否含有条子
		auto it_tiaozi = std::find_if(gang_list.begin(), gang_list.end(), [](const Asset::PaiElement& pai){
					return pai.card_type() == Asset::CARD_TYPE_TIAOZI;
				});
		if (cards[Asset::CARD_TYPE_TIAOZI].size() > 0 || it_bingzi != gang_list.end()) ++has_count; 

		auto it_duanmen = std::find(options.extend_type().begin(), options.extend_type().end(), Asset::ROOM_EXTEND_TYPE_DUANMEN);
		if (it_duanmen == options.extend_type().end()) //不可以缺门
		{
			if (has_count < 3) //少于三门显然不行，检查是否清一色
			{
				auto it_yise = std::find(options.extend_type().begin(), options.extend_type().end(), Asset::ROOM_EXTEND_TYPE_QIYISE);
				if (it_yise != options.extend_type().end()) //可以清一色
				{
					if (has_count == 2) //有两门显然不是清一色
					{
						spdlog::get("console")->debug("{0} Line:{1} player_id:{2} card_type:{3} card_value:{4} reason:缺门也不是清一色.", 
								__func__, __LINE__, GetID(), pai.card_type(), pai.card_value());
						return false; //不可缺门
					}
					else // <= 1
					{
						yise = true; //是清一色
					}
				}
				else //断门还不可以清一色
				{
					spdlog::get("console")->debug("{0} Line:{1} player_id:{2} card_type:{3} card_value:{4} reason:缺门.", 
							__func__, __LINE__, GetID(), pai.card_type(), pai.card_value());
					return false;
				}
			}
		}
		else //可以断门
		{
			if (has_count < 3) duanmen = true;

			auto it_yise = std::find(options.extend_type().begin(), options.extend_type().end(), Asset::ROOM_EXTEND_TYPE_QIYISE);
			if (it_yise != options.extend_type().end()) //可以清一色
			{
				if (has_count <= 1) 
				{
					yise = true;
					duanmen = false; //清一色不算断门
				}
			}
		}
	}

	////////是否可以站立胡
	{
		auto it_zhanli = std::find(options.extend_type().begin(), options.extend_type().end(), Asset::ROOM_EXTEND_TYPE_ZHANLIHU);
		if (it_zhanli == options.extend_type().end()) //不可以站立胡牌
		{
			if (_cards_outhand.size() == 0 && _minggang.size() == 0) 
			{
				spdlog::get("console")->debug("{0} Line:{1} player_id:{2} card_type:{3} card_value:{4} reason:没开门.", 
						__func__, __LINE__, GetID(), pai.card_type(), pai.card_value());
				return false; //没开门
			}
		}
		else
		{
			if (_cards_outhand.size() == 0 && _minggang.size() == 0) zhanlihu = true;
		}
	}
	
	////////是否有幺九
	bool has_yao = false;

	for (auto crds : cards) //不同牌类别的牌
	{
		if (crds.second.size() == 0) continue;

		if (crds.first == Asset::CARD_TYPE_WANZI || crds.first == Asset::CARD_TYPE_BINGZI || crds.first == Asset::CARD_TYPE_TIAOZI)
		{
			if (std::find(crds.second.begin(), crds.second.end(), 1) != crds.second.end() || 
					(std::find(crds.second.begin(), crds.second.end(), 9) != crds.second.end()))
			{
				has_yao = true;
				break;
			}
		}
		
		if (crds.first == Asset::CARD_TYPE_FENG || crds.first == Asset::CARD_TYPE_JIAN)
		{
			has_yao = true;
			break;
		}
	}

	for (auto gang : _minggang)
	{
		if (gang.card_value() == 1 || gang.card_value() == 9) has_yao = true;
	}
	
	for (auto gang : _angang)
	{
		if (gang.card_value() == 1 || gang.card_value() == 9) has_yao = true;
	}

	if (_jiangang > 0 || _fenggang > 0) 
	{
		has_yao = true;
		xuanfenggang = true;
	}

	if (!has_yao) 
	{
		spdlog::get("console")->debug("{0} Line:{1} player_id:{2} card_type:{3} card_value:{4} reason:没幺九.", 
				__func__, __LINE__, GetID(), pai.card_type(), pai.card_value());
		return false;
	}

	////////是否可以宝胡
	/*
	{
		auto it_baohu = std::find(options.extend_type().begin(), options.extend_type().end(), Asset::ROOM_EXTEND_TYPE_BAOPAI);
		if (it_baohu != options.extend_type().end()) //可以宝胡
		{
			baohu = CheckBaoHu(pai);
		}
	}
	*/

	////////////////////////////////////////////////////////////////////////////是否可以满足胡牌的要求
	
	hu_result.clear();

	std::vector<Card_t> card_list;
	for (auto crds : cards) //不同牌类别的牌
	{
		for (auto value : crds.second)
			card_list.push_back(Card_t(crds.first, value));
	}

	/*
	std::cout << "转换后的玩家手里的牌:" << std::endl;
	for (auto card : card_list)
	{
		std::cout << card.card_type << " " << card.card_value << std::endl;
	}
	*/

	bool can_hu = CanHuPai(card_list);	
	if (!can_hu && !baohu) 
	{
		spdlog::get("console")->debug("{0} Line:{1} player_id:{2} card_type:{3} card_value:{4} reason:自己牌内无法满足胡牌条件.", 
				__func__, __LINE__, GetID(), pai.card_type(), pai.card_value());
		return false;
	}
	
	//胡牌时至少有一刻子或杠，或有中发白其中一对
	bool has_ke = false;
	int32_t ke_count = 0; //刻数量，一般最多4个，即12张

	for (auto r : hu_result)
	{
		 bool is_ke = std::get<1>(r);
		 if (is_ke) ++ke_count;
	}

	if (ke_count) has_ke = true;

	if (!has_ke && (cards[Asset::CARD_TYPE_FENG].size() || cards[Asset::CARD_TYPE_JIAN].size())) has_ke = true;
	
	if (!has_ke && (_jiangang > 0 || _fenggang > 0 || _minggang.size() > 0 || _angang.size() > 0)) has_ke = true;
	
	if (!has_ke) 
	{
		spdlog::get("console")->debug("{0} Line:{1} player_id:{2} card_type:{3} card_value:{4} reason:没有刻.", 
				__func__, __LINE__, GetID(), pai.card_type(), pai.card_value());
		return false;
	}

	auto ke_total = ke_count + _jiangang + _fenggang + _minggang.size() + _angang.size();
	if (ke_total == 4) piao = true; //TODO：玩家吃了三套副一样的..

	////////是否是夹胡
	{
		bool he_deleted = false, smaller_deleted = false, bigger_deleted = false;

		for (auto it = card_list.begin(); it != card_list.end();)
		{
			if (it->_type == pai.card_type() && it->_value == pai.card_value() - 1)
			{
				if (!smaller_deleted) it = card_list.erase(it); //只删除一个
				smaller_deleted = true;
			}
			else if (it->_type == pai.card_type() && it->_value == pai.card_value())
			{
				if (!he_deleted) it = card_list.erase(it); //只删除一个
				he_deleted = true;
			}
			else if (it->_type == pai.card_type() && it->_value == pai.card_value() + 1)
			{
				if (!bigger_deleted) it = card_list.erase(it); //只删除一个
				bigger_deleted = true;
			}
			else
			{
				++it;
			}
		}

		if (smaller_deleted && he_deleted && bigger_deleted)
		{
			bool can_hu = CanHuPai(card_list);	
			if (can_hu) jiahu = true; //如果删除了这个顺子还能胡，就说明真的是夹胡
		}
	}

	////////////////////////////////////////////////////////////////////////////积分计算
	
	//base_score = 1; //基础分


	if (zhanlihu)
	{
		fan_list.push_back(Asset::FAN_TYPE_ZHAN_LI);
		//base_score *= 2; //是否站立胡
	}
	if (duanmen) 
	{
		fan_list.push_back(Asset::FAN_TYPE_DUAN_MEN);
		//base_score *= 2; //是否缺门
	}
	if (yise) 
	{
		fan_list.push_back(Asset::FAN_TYPE_QING_YI_SE);
		//base_score *= 2; //是否清一色
	}
	if (baopai) 
	{
		fan_list.push_back(Asset::FAN_TYPE_LOU_BAO);
		//base_score *= 2; //是否宝牌
	}
	if (piao) 
	{
		fan_list.push_back(Asset::FAN_TYPE_PIAO_HU);
		//base_score *= 2; //是否飘胡
	}
	if (xuanfenggang) //是否旋风杠
	{
		for (auto i = 0; i < _jiangang + _fenggang; ++i) 
		{
			fan_list.push_back(Asset::FAN_TYPE_XUAN_FENG_GANG);
			//base_score *= 2;
		}
	}
	if (jiahu) //夹胡积分
	{
		auto it_jiahu = std::find(options.extend_type().begin(), options.extend_type().end(), Asset::ROOM_EXTEND_TYPE_JIAHU);
		if (it_jiahu != options.extend_type().end()) //普通夹胡
		{
			fan_list.push_back(Asset::FAN_TYPE_JIA_HU_NORMAL);
			//base_score *= 2;
		}
		else
		{
			if (pai.card_value() == 3 || pai.card_value() == 7) 
			{
				fan_list.push_back(Asset::FAN_TYPE_JIA_HU_MIDDLE);
				//base_score *= 4;
			}
			if (pai.card_value() == 5) 
			{
				fan_list.push_back(Asset::FAN_TYPE_JIA_HU_HIGHER);
				//base_score *= 8;
			}
		}
	}
	/*
	if (baohu)
	{
		fan_list.push_back(Asset::FAN_TYPE_LOU_BAO);
	}
	*/

	return true;
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
	if (!CheckChiPai(pai) || !message) 
	{
		DEBUG_ASSERT(false);
		return;
	}

	Asset::PaiOperation* pai_operate = dynamic_cast<Asset::PaiOperation*>(message);
	if (!pai_operate) return;
	
	std::vector<Asset::PaiElement> cards;

	cards.push_back(pai);

	if (pai_operate->pais().size() != 2) 
	{
		DEBUG_ASSERT(false);
		return; 
	}
	
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

	if (cards[1].card_value() - cards[0].card_value() != 1 || cards[2].card_value() - cards[1].card_value() != 1) 
	{
		DEBUG_ASSERT(false);
		return; //不是顺子
	}

	auto first = std::find(it->second.begin(), it->second.end(), pai_operate->pais(0).card_value());
	if (first == it->second.end()) 
	{
		DEBUG_ASSERT(false);
		return; //理论上不会出现
	}
	
	DEBUG("%s:line:%d,删除牌 类型:%d--值%d", __func__, __LINE__, pai_operate->pais(0).card_type(), pai_operate->pais(0).card_value());
	it->second.erase(first); //删除

	auto second = std::find(it->second.begin(), it->second.end(), pai_operate->pais(1).card_value());
	if (second == it->second.end()) 
	{
		DEBUG_ASSERT(false);
		return; //理论上不会出现
	}

	DEBUG("%s:line:%d,删除牌 类型:%d--值%d", __func__, __LINE__, pai_operate->pais(1).card_type(), pai_operate->pais(0).card_value());
	it->second.erase(second); //删除
	
	for (auto card : cards)
		_cards_outhand[card.card_type()].push_back(card.card_value());

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
	if (!CheckPengPai(pai)) 
	{
		DEBUG_ASSERT(false);
		DEBUG("%s:line:%d,玩家无法碰牌 类型:%d--值%d\n", __func__, __LINE__, GetID(), pai.card_type(), pai.card_value());
		return;
	}
	
	auto it = _cards.find(pai.card_type());
	if (it == _cards.end()) return; //理论上不会如此
	
	for (int i = 0; i < 2; ++i)
	{
		auto iit = std::find(it->second.begin(), it->second.end(), pai.card_value()); //从玩家手里删除
		if (iit == it->second.end()) return;
		it->second.erase(iit);
		DEBUG("%s:line:%d,删除玩家%ld手中牌 类型:%d--值%d\n", __func__, __LINE__, GetID(), pai.card_type(), pai.card_value());
	}

	for (int i = 0; i < 3; ++i)
	{
		_cards_outhand[pai.card_type()].push_back(pai.card_value());
	}
	
	SynchronizePai();
}

bool Player::CheckGangPai(const Asset::PaiElement& pai, int64_t from_player_id)
{
	auto it = _cards.find(pai.card_type());
	int32_t card_value = pai.card_value();

	if (it != _cards.end()) 
	{
		int32_t count = std::count(it->second.begin(), it->second.end(), card_value);
		if (count == 3 /*牌是来自其他玩家*/ || count == 4 /*牌是玩家自己抓的*/) return true;  //玩家手里需要有3|4张牌
	}

	DEBUG("%s:line:%d,杠牌 玩家%ld墙外牌: 类型:%d 值:%d from_player_id:%ld\n", 
			__func__, __LINE__, GetID(), pai.card_type(), pai.card_value(), from_player_id);

	if (from_player_id == GetID()) //玩家自己抓牌
	{
		auto it = _cards_outhand.find(pai.card_type()); //牌面的牌不做排序,顺序必须3张

		auto first_it = std::find(it->second.begin(), it->second.end(), card_value);

		if (first_it == it->second.end()) 
		{
			DEBUG("%s:line:%d,杠牌 玩家%ld墙外牌: 类型:%d, 值:%d from_player_id:%ld\n", 
				__func__, __LINE__, GetID(), pai.card_type(), pai.card_value(), from_player_id);
			return false;
		}

		auto second_it = ++first_it;
		if (second_it == it->second.end())
		{
			DEBUG("%s:line:%d,杠牌 玩家%ld墙外牌: 类型:%d, 值:%d from_player_id:%ld\n", 
				__func__, __LINE__, GetID(), pai.card_type(), pai.card_value(), from_player_id);
			return false;
		}

		auto third_it = ++second_it;
		if (third_it == it->second.end())
		{
			DEBUG("%s:line:%d,杠牌 玩家%ld墙外牌: 类型:%d, 值:%d from_player_id:%ld\n", 
				__func__, __LINE__, GetID(), pai.card_type(), pai.card_value(), from_player_id);
			return false;
		}
		
		DEBUG("%s:line:%d,杠牌 玩家%ld墙外牌: 类型:%d, 值:%d %d %d %d\n", 
				__func__, __LINE__, GetID(), pai.card_type(), pai.card_value(), *first_it, *second_it, *third_it);

		if ((*first_it == *second_it) && (*second_it == *third_it)) return true;  //玩家牌面有3张牌
	}
		
	DEBUG("%s:line:%d,玩家%ld不能杠牌, 类型:%d, 值:%d\n", __func__, __LINE__, GetID(), pai.card_type(), pai.card_value());

	return false;
}

bool Player::CheckAllGangPai(std::vector<Asset::PaiElement>& pais)
{
	/////手里有4张牌，即暗杠检查
	for (auto cards : _cards)
	{
		auto card_type = cards.first;

		for (auto card_value : cards.second)
		{
			auto count = std::count(cards.second.begin(), cards.second.end(), card_value);
			if (count == 4) 
			{
				Asset::PaiElement pai;
				pai.set_card_type((Asset::CARD_TYPE)card_type);
				pai.set_card_value(card_value);

				auto it = std::find_if(pais.begin(), pais.end(), [card_type, card_value](const Asset::PaiElement& pai){
							return card_type == pai.card_type() && card_value == pai.card_value();
						});
				if (it == pais.end()) pais.push_back(pai); //暗杠
			}
		}
	}

	/////手里有1张牌，牌面有3张碰牌，即明杠检查
	for (auto cards : _cards_outhand)
	{
		DEBUG_ASSERT(cards.second.size() % 3 == 0);

		int32_t card_type = cards.first;

		//auto size = cards.second.size() - 3;

		for (size_t i = 0; i < cards.second.size(); i = i + 3)
		{
			auto card_value = cards.second.at(i);

			if ((card_value != cards.second.at(i + 1)) || (card_value != cards.second.at(i + 2))) continue; //外面是否碰了3张

			auto it = _cards.find(card_type);
			if (it == _cards.end()) continue;
			
			auto iit = std::find(it->second.begin(), it->second.end(), card_value);
			if (iit == it->second.end()) continue; //手里有1张才满足
			
			Asset::PaiElement pai;
			pai.set_card_type((Asset::CARD_TYPE)card_type);
			pai.set_card_value(card_value);

			pais.push_back(pai); //明杠
		}
	}

	return pais.size() > 0;
}
	
void Player::OnGangPai(const Asset::PaiElement& pai, int64_t from_player_id)
{
	if (!CheckGangPai(pai, from_player_id)) 
	{
		DEBUG_ASSERT(false);
		return;
	}
	
	int32_t card_type = pai.card_type();
	int32_t card_value = pai.card_value();

	/////////////////////////////////////////////////////////////////////////////手里满足杠牌
	auto it = _cards.find(card_type);
	if (it == _cards.end()) 
	{
		DEBUG_ASSERT(false);
		return; //理论上不会如此
	}
	
	auto count = std::count(it->second.begin(), it->second.end(), card_value); //玩家手里多少张牌

	if (count == 3)
		_minggang.push_back(pai);
	else if (count == 4)
		_angang.push_back(pai);
	
	auto remove_it = std::remove(it->second.begin(), it->second.end(), card_value); //从玩家手里删除
	it->second.erase(remove_it, it->second.end());
	
	/////////////////////////////////////////////////////////////////////////////墙外满足杠牌
	auto iit = _cards_outhand.find(card_type);
	if (iit != _cards_outhand.end()) 
	{
		auto count = std::count(iit->second.begin(), iit->second.end(), card_value); //玩家手里多少张牌

		if (count == 3)
		{
			_minggang.push_back(pai);
			
			auto remove_it = std::remove(iit->second.begin(), iit->second.end(), card_value); //从墙外删除
			iit->second.erase(remove_it, iit->second.end());
		}
	}
	
	//记录日志
	P(Asset::ACTION, "%s:line:%d, player:%ld 玩家杠牌, 牌类型:%d, 牌值:%d, 数量:%d.", __func__, __LINE__, GetID(), pai.card_type(), pai.card_value(), count);
	
	DEBUG("%s:line:%d, player:%ld 玩家杠牌, 牌类型:%d, 牌值:%d, 数量:%d.", __func__, __LINE__, GetID(), pai.card_type(), pai.card_value(), count);
	
	//从后楼给玩家取一张牌
	auto cards = _game->TailPai(1);
	OnFaPai(cards);
	
	SynchronizePai();
}

bool Player::CheckFengGangPai() 
{ 
	if (_stuff.player_prop().check_feng_gang()) return false;

	_stuff.mutable_player_prop()->set_check_feng_gang(true); //设置已经检查过旋风杠

	return CheckFengGangPai(_cards); 
}

bool Player::CheckJianGangPai() 
{ 
	if (_stuff.player_prop().check_jian_gang()) return false;

	_stuff.mutable_player_prop()->set_check_jian_gang(true); //设置已经检查过旋风杠

	return CheckJianGangPai(_cards); 
}
	
bool Player::CheckTingPai()
{
	auto options = _locate_room->GetOptions();
	
	auto it_baohu = std::find(options.extend_type().begin(), options.extend_type().end(), Asset::ROOM_EXTEND_TYPE_BAOPAI);
	if (it_baohu == options.extend_type().end()) return false; //不带宝胡，绝对不可能呢听牌

	for (int card_type = Asset::CARD_TYPE_WANZI; card_type <= Asset::CARD_TYPE_TIAOZI; ++card_type)
	{
		for (int card_value = 1; card_value <= 9; ++card_value)
		{
			Asset::PaiElement pai;
			pai.set_card_type((Asset::CARD_TYPE)card_type);
			pai.set_card_value(card_value);

			std::vector<Asset::FAN_TYPE> fan_list;
			if (CheckHuPai(pai, fan_list)) return true;
		}
	}
	
	for (int card_type = Asset::CARD_TYPE_FENG; card_type <= Asset::CARD_TYPE_JIAN; ++card_type)
	{
		for (int card_value = 1; card_value <= 9; ++card_value)
		{
			Asset::PaiElement pai;
			pai.set_card_type((Asset::CARD_TYPE)card_type);
			pai.set_card_value(card_value);

			std::vector<Asset::FAN_TYPE> fan_list;
			if (CheckHuPai(pai, fan_list)) return true;
		}
	}

	return false;
}

bool Player::CheckFengGangPai(std::map<int32_t/*麻将牌类型*/, std::vector<int32_t>/*牌值*/>& cards)
{
	if (!_locate_room) return false;

	auto options = _locate_room->GetOptions();

	auto it_xuanfeng = std::find(options.extend_type().begin(), options.extend_type().end(), Asset::ROOM_EXTEND_TYPE_XUANFENGGANG);
	if (it_xuanfeng == options.extend_type().end()) return false; //不支持旋风杠

	auto it = cards.find(Asset::CARD_TYPE_FENG);
	if (it == cards.end()) return false;

	for (int32_t card_value = 1; card_value <= 4; ++card_value) //东南西北
	{
		auto it_if = std::find(it->second.begin(), it->second.end(), card_value);
		if (it_if == it->second.end()) return false;
	}
	return true;
}

void Player::OnGangFengPai()
{
	if (!CheckFengGangPai(_cards)) return;

	auto it = _cards.find(Asset::CARD_TYPE_FENG);
	if (it == _cards.end()) return;

	for (int32_t card_value = 1; card_value <= 4; ++card_value) //东南西北
	{
		auto it_if = std::find(it->second.begin(), it->second.end(), card_value);
		if (it_if != it->second.end())  it->second.erase(it_if); //删除
	}

	++_fenggang;
	
	P(Asset::ACTION, "%s:line:%d, player:%ld 旋风杠", __func__, __LINE__, GetID());
	
	//从后楼给玩家取一张牌
	auto cards = _game->TailPai(1);
	OnFaPai(cards);
}

bool Player::CheckJianGangPai(std::map<int32_t/*麻将牌类型*/, std::vector<int32_t>/*牌值*/>& cards)
{
	if (!_locate_room) return false;

	auto options = _locate_room->GetOptions();

	auto it_xuanfeng = std::find(options.extend_type().begin(), options.extend_type().end(), Asset::ROOM_EXTEND_TYPE_XUANFENGGANG);
	if (it_xuanfeng == options.extend_type().end()) return false; //不支持

	auto it = cards.find(Asset::CARD_TYPE_JIAN);
	if (it == cards.end()) return false;

	for (auto card_value = 1; card_value <= 3; ++card_value) //中发白
	{
		auto it_if = std::find(it->second.begin(), it->second.end(), card_value);
		if (it_if == it->second.end()) return false;
	}
	return true;
}

void Player::OnGangJianPai()
{
	if (!CheckJianGangPai(_cards)) return;

	auto it = _cards.find(Asset::CARD_TYPE_JIAN);
	for (auto card_value = 1; card_value <= 3; ++card_value) //中发白
	{
		auto it_if = std::find(it->second.begin(), it->second.end(), card_value);

		if (it_if != it->second.end())  it->second.erase(it_if); //删除
	}

	++_jiangang;

	P(Asset::ACTION, "%s:line:%d, player:%ld 旋风杠", __func__, __LINE__, GetID());
}

int32_t Player::OnFaPai(std::vector<int32_t>& cards)
{
	auto cards_inhand = _cards;

	for (auto card_index : cards)
	{
		auto card = GameInstance.GetCard(card_index);
		if (card.card_type() == 0 || card.card_value() == 0) return 1; //数据有误

		_cards[card.card_type()].push_back(card.card_value()); //插入玩家手牌
	}

	for (auto& cards : _cards) //整理牌
	{
		std::sort(cards.second.begin(), cards.second.end(), [](int x, int y){ return x < y; }); //由小到大
	}

	Asset::PaiNotify notify; /////玩家当前牌数据发给Client

	if (cards.size() > 1) //开局
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

		SynchronizePai(); //每次都同步

		if (IsTingPai())
		{
			auto count = GetCountAfterTingOperation();
			if (count == 1) //听牌后第一次抓牌
			{
				Asset::RandomSaizi proto;
				int32_t result = CommonUtil::Random(1, 6);
				proto.set_random_result(result);

				auto baopai = _game->GetBaopai(result);
				_game->SetBaoPai(baopai);

				proto.mutable_pai()->CopyFrom(baopai);

				//宝牌
				_game->BroadCast(proto);
			}

			Asset::PaiOperation pai_operation; //如果听牌，自动给玩家出牌
			pai_operation.set_oper_type(Asset::PAI_OPER_TYPE_DAPAI);
			pai_operation.set_position(GetPosition());
			pai_operation.mutable_pai()->CopyFrom(card);
			CmdPaiOperate(&pai_operation);

			IncreaseTingOperationCount();
		}
	}
	
	SendProtocol(notify); //发送

	return 0;
}
	
void Player::SynchronizePai()
{
	return;

	Asset::PaiNotify notify; /////玩家当前牌数据发给Client

	for (auto pai : _cards)
	{
		auto pais = notify.mutable_pais()->Add();

		pais->set_card_type((Asset::CARD_TYPE)pai.first); //牌类型

		DEBUG("%s:line:%d, 玩家 %ld手里真正的牌数据，牌类型:%d, 牌值:", __func__, __LINE__, pai.first);

		for (auto value : pai.second)
			std::cout << value << " ";
		std::cout << std::endl;

		::google::protobuf::RepeatedField<int32_t> cards(pai.second.begin(), pai.second.end());
		pais->mutable_cards()->CopyFrom(cards); //牌值
	}
	
	notify.set_data_type(Asset::PaiNotify_CARDS_DATA_TYPE_CARDS_DATA_TYPE_SYNC); //操作类型：同步数据
	
	SendProtocol(notify); //发送
}

void Player::PrintPai()
{
	Asset::PaiNotify notify; /////玩家当前牌数据发给Client
	
	for (auto pai : _minggang)
	{
		DEBUG("%s:line:%d, 玩家 %ld明杠真正的牌数据，牌类型:%d, 牌值:%d\n", __func__, __LINE__, GetID(), pai.card_type(), pai.card_value());
	}
	
	for (auto pai : _angang)
	{
		DEBUG("%s:line:%d, 玩家 %ld暗杠真正的牌数据，牌类型:%d, 牌值:%d\n", __func__, __LINE__, GetID(), pai.card_type(), pai.card_value());
	}
	
	for (auto pai : _cards_outhand)
	{
		DEBUG("%s:line:%d, 玩家 %ld牌外真正的牌数据，牌类型:%d, 牌值:", __func__, __LINE__, GetID(), pai.first);

		for (auto value : pai.second)
			std::cout << value << " ";
		std::cout << std::endl;
	}

	for (auto pai : _cards)
	{
		auto pais = notify.mutable_pais()->Add();

		pais->set_card_type((Asset::CARD_TYPE)pai.first); //牌类型
		
		DEBUG("%s:line:%d, 玩家 %ld手里真正的牌数据，牌类型:%d, 牌值:", __func__, __LINE__, GetID(), pai.first);

		for (auto value : pai.second)
			std::cout << value << " ";
		std::cout << std::endl;

		::google::protobuf::RepeatedField<int32_t> cards(pai.second.begin(), pai.second.end());
		pais->mutable_cards()->CopyFrom(cards); //牌值
	}
	
	notify.set_data_type(Asset::PaiNotify_CARDS_DATA_TYPE_CARDS_DATA_TYPE_SYNC); //操作类型：同步数据
	
	std::cout << "当前玩家牌:" << GetID() << std::endl;
	notify.PrintDebugString();
}

void Player::ClearCards() 
{
	_cards.clear();	
	_cards_outhand.clear();

	_minggang.clear();
	_angang.clear();

	_jiangang = 0;
	_fenggang = 0;
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
