#pragma once

#include <map>
#include <mutex>
#include <cmath>
#include <memory>
#include <unordered_map>
#include <functional>

#include "P_Header.h"
#include "Item.h"
#include "Asset.h"
#include "WorldSession.h"
#include "MessageDispatcher.h"

namespace Adoter
{
namespace pb = google::protobuf;

class Room;
class Game;

class Player : public std::enable_shared_from_this<Player>
{
	typedef std::function<int32_t(pb::Message*)> CallBack;
	std::unordered_map<int32_t, CallBack>  _callbacks;	//每个协议的回调函数，不要传入引用
private:
	Asset::Player _stuff; //玩家数据
	int64_t _heart_count = 0; //心跳次数

	CallBack _method;
	std::shared_ptr<WorldSession> _session = nullptr;	//网络连接
public:
	Player();
	Player(int64_t player_id, std::shared_ptr<WorldSession> session);
	//协议处理默认调用函数
	int32_t DefaultMethod(pb::Message*);
	
	void AddHandler(Asset::META_TYPE message_type, CallBack callback)
	{
		if (_callbacks.find(message_type) != _callbacks.end()) return;

		_callbacks.emplace(message_type, callback);
	}

	CallBack& GetMethod(int32_t message_type)
	{
		auto it = _callbacks.find(message_type);
		if (it == _callbacks.end()) return _method;
		return it->second;
	}

	//获取基础属性
	Asset::Player& Get() { return _stuff; }
	const Asset::CommonProp& CommonProp() { return _stuff.common_prop(); }
	const Asset::CommonProp& GetCommonProp() { return _stuff.common_prop(); }
	Asset::CommonProp* MutableCommonProp() { return _stuff.mutable_common_prop(); }
	//发送玩家基础属性
	void SyncCommonProperty();
	//获取ID
	virtual int64_t GetID() { return _stuff.common_prop().player_id(); }
	virtual void SetID(int64_t player_id) { 
		_stuff.mutable_common_prop()->set_player_id(player_id); 
	} 
	//获取级别
	virtual int32_t GetLevel() { return _stuff.common_prop().level(); }
	//获取性别
	virtual int32_t GetGender() { return _stuff.common_prop().gender(); }
	//消息处理
	virtual bool HandleMessage(const Asset::MsgItem& item); 
	virtual void SendMessage(Asset::MsgItem& item);
	virtual void BroadCastCommonProp(Asset::MSG_TYPE type); //向房间里的玩家发送公共数据       
	//协议处理(Protocol Buffer)
	virtual bool HandleProtocol(int32_t type_t, pb::Message* message);
	virtual void SendProtocol(pb::Message& message);
	virtual void SendProtocol(pb::Message* message);
	virtual void SendResponse(pb::Message* message);
	virtual void SendToRoomers(pb::Message& message); //向房间里玩家发送协议数据，发送到客户端
	virtual void BroadCast(Asset::MsgItem& item);
	//virtual void OnCreatePlayer(int64_t player_id);
	//进入游戏
	virtual int32_t CmdEnterGame(pb::Message* message);
	virtual int32_t OnEnterGame();
	//创建房间
	virtual int32_t CmdCreateRoom(pb::Message* message);
	virtual void OnCreateRoom(int64_t room_id);
	//进入房间
	virtual int32_t CmdEnterRoom(pb::Message* message);
	virtual bool OnEnterRoom(int64_t room_id = 0);
	//玩家登录
	virtual int32_t CmdLogin(pb::Message* message);
	//玩家登出
	virtual int32_t CmdLogout(pb::Message* message);
	//离开房间
	virtual int32_t CmdLeaveRoom(pb::Message* message);
	virtual void OnLeaveRoom();
	//加载数据	
	virtual int32_t Load();
	//保存数据
	virtual int32_t Save();
	//同步玩家数据
	virtual void SendPlayer();
	//玩家心跳周期为10MS，如果该函数返回FALSE则表示掉线
	virtual bool Update();
	//购买商品
	virtual bool CmdBuySomething(pb::Message* message);
public:
	//获取所有包裹
	const Asset::Inventory& GetInventory()
	{
		return _stuff.inventory();
	}

	//获取指定包裹
	const Asset::Inventory_Element& GetInventory(Asset::INVENTORY_TYPE type)
	{
		return _stuff.inventory().inventory(type);
	}	
	
	//获取物品
	bool GainItem(Item* item, int32_t count = 1);
	bool GainItem(int64_t global_item_id, int32_t count = 1);
	//存放物品
	bool PushBackItem(Asset::INVENTORY_TYPE inventory_type, Item* item);

	const std::shared_ptr<WorldSession> GetSession()
	{
		return _session;
	}
	//发送错误信息
	void AlertMessage(Asset::ERROR_CODE error_code, Asset::ERROR_TYPE error_type = Asset::ERROR_TYPE_NORMAL, Asset::ERROR_SHOW_TYPE error_show_type = Asset::ERROR_SHOW_TYPE_CHAT);

	//消费欢乐豆：返回实际消耗的欢乐豆数
	int64_t ConsumeHuanledou(int64_t count)
	{
		if (count <= 0) return 0;

		if (!CheckHuanledou(count)) return 0;

		_stuff.mutable_common_prop()->set_huanledou(_stuff.common_prop().huanledou() - count);
		
		SyncCommonProperty();
		
		return count;
	}
	//增加欢乐豆
	int64_t IncreaseHuanledou(int64_t count)
	{
		if (count <= 0) return 0;

		_stuff.mutable_common_prop()->set_huanledou(_stuff.common_prop().huanledou() + count);
		
		SyncCommonProperty();
		
		return count;
	}
	//欢乐豆是否足够
	bool CheckHuanledou(int64_t count)
	{
		int64_t curr_count = _stuff.common_prop().huanledou();
		return curr_count >= count;
	}
	//获取欢乐豆数量
	int64_t GetHuanledou() { return _stuff.common_prop().huanledou(); }
	//获取钻石数量
	int64_t GetDiamond() { return _stuff.common_prop().diamond(); }

	//消费钻石：返回实际消耗的钻石数
	int64_t ConsumeDiamond(int64_t count)
	{
		if (count <= 0) return 0;

		if (!CheckDiamond(count)) return 0;

		_stuff.mutable_common_prop()->set_diamond(_stuff.common_prop().diamond() - count);
		
		SyncCommonProperty();
		
		return count;
	}
	//增加钻石
	int64_t IncreaseDiamond(int64_t count)
	{
		if (count >= 0) return 0;

		_stuff.mutable_common_prop()->set_diamond(_stuff.common_prop().diamond() + count);

		SyncCommonProperty();
		
		return count;
	}
	//钻石是否足够
	bool CheckDiamond(int64_t count)
	{
		int64_t curr_count = _stuff.common_prop().diamond();
		return curr_count >= count;
	}
	//通用限制
	const Asset::PlayerCommonLimit& GetCommonLimit() {
		return _stuff.common_limit();
	}
	
	Asset::PlayerCommonLimit* GetMutableCommonLimit() {
		return _stuff.mutable_common_limit();
	}
	bool AddCommonLimit(int64_t global_id);
	bool IsCommonLimit(int64_t global_id);
	bool CommonLimitUpdate();
	void SyncCommonLimit();
	//通用奖励
	bool DeliverReward(int64_t global_id);
	void SyncCommonReward(int64_t common_reward_id);
///////游戏逻辑定义
private:
	std::shared_ptr<Room> _locate_room = nullptr; //实体所在房间
	std::shared_ptr<Game> _game = nullptr; //当前游戏
	std::unordered_map<int32_t/*麻将牌类型*/, std::vector<int32_t>/*牌值*/> _cards; //玩家具有的麻将
public:
	//玩家操作
	virtual int32_t CmdGameOperate(pb::Message* message); //游戏操作
	virtual int32_t CmdPaiOperate(pb::Message* message); //牌操作
	virtual int32_t CmdGetReward(pb::Message* message); //领取奖励
	//获取房间
	virtual std::shared_ptr<Room> GetRoom() { return _locate_room; }	//获取当前房间
	virtual void SetRoomID(int64_t room_id) { _stuff.set_room_id(room_id); }	
	virtual int32_t GetRoomID() { return _stuff.room_id(); }
	virtual bool HasRoom() { return _locate_room != nullptr; }

	void SetGame(std::shared_ptr<Game> game) { _game = game; }

	virtual int32_t OnFaPai(std::vector<int32_t> cards); //发牌

	Asset::PAI_CHECK_RETURN CheckPai(const Asset::PaiElement& pai);

	bool CheckHuPai(const Asset::PaiElement& pai); //胡牌

	bool CheckGangPai(const Asset::PaiElement& pai); //是否可以杠牌
	void OnGangPai(const Asset::PaiElement& pai); //杠牌

	bool CheckPengPai(const Asset::PaiElement& pai); //是否可以碰牌
	void OnPengPai(const Asset::PaiElement& pai); //碰牌

	bool CheckChiPai(const Asset::PaiElement& pai); //是否可以吃牌
	void OnChiPai(const Asset::PaiElement& pai, pb::Message* message); //吃牌
	//是否已经在准备状态 
	bool IsReady() { return _stuff.player_prop().game_oper_state() == Asset::GAME_OPER_TYPE_START; }
	//获取玩家座次
	Asset::POSITION_TYPE GetPosition() { return _stuff.player_prop().position(); }

	void SynchronizePai();
	void ClearCards() {	_cards.clear();	}
};

/////////////////////////////////////////////////////
//玩家通用管理类
/////////////////////////////////////////////////////
class PlayerManager : public std::enable_shared_from_this<PlayerManager>
{
private:
	std::mutex _mutex;
	std::unordered_map<int64_t, std::shared_ptr<Player>> _entities; //实体为智能指针，不要传入引用
public:
	static PlayerManager& Instance()
	{
		static PlayerManager _instance;
		return _instance;
	}

	void Erase(int64_t entity_id);
	void Erase(Player& entity);
	void Add(std::shared_ptr<Player> entity);
	void Emplace(int64_t entity_id, std::shared_ptr<Player> entity);
	bool Has(int64_t entity_id);
	std::shared_ptr<Player> GetPlayer(int64_t id);
};

#define PlayerInstance PlayerManager::Instance()

}
