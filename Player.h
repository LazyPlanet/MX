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
	bool _dirty = false;

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

	//获取玩家数据
	Asset::Player& Get() { return _stuff; }
	std::string GetString();
	//获取基础属性
	const Asset::CommonProp& CommonProp() { return _stuff.common_prop(); }
	const Asset::CommonProp& GetCommonProp() { return _stuff.common_prop(); }
	Asset::CommonProp* MutableCommonProp() { return _stuff.mutable_common_prop(); }
	//发送玩家基础属性
	void SyncCommonProperty();
	//获取ID
	virtual int64_t GetID() { return _stuff.common_prop().player_id(); }
	virtual void SetID(int64_t player_id) { _stuff.mutable_common_prop()->set_player_id(player_id); } 
	//获取名字
	virtual std::string GetName() { return _stuff.common_prop().name(); }
	virtual void SetName(std::string name) { _stuff.mutable_common_prop()->set_name(name); } 
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
	//virtual void SendToRoomers(pb::Message& message); //向房间里玩家发送协议数据，发送到客户端
	virtual void BroadCast(Asset::MsgItem& item);
	//virtual void OnCreatePlayer(int64_t player_id);
	//进入游戏
	//virtual int32_t CmdEnterGame(pb::Message* message);
	virtual int32_t OnEnterGame();
	//创建房间
	virtual int32_t CmdCreateRoom(pb::Message* message);
	virtual void OnCreateRoom(Asset::CreateRoom* create_room);
	//进入房间
	virtual int32_t CmdEnterRoom(pb::Message* message);
	virtual bool OnEnterRoom(int64_t room_id = 0);
	//玩家登录
	virtual int32_t OnLogin(pb::Message* message);
	//玩家登出
	virtual int32_t OnLogout(pb::Message* message);
	//离开房间
	virtual int32_t CmdLeaveRoom(pb::Message* message);
	virtual void OnLeaveRoom();
	//加载数据	
	virtual int32_t Load();
	//保存数据
	virtual int32_t Save();
	//是否脏数据
	virtual bool IsDirty() { return _dirty; }
	virtual void SetDirty() { _dirty = true; }
	//同步玩家数据
	virtual void SendPlayer();
	//玩家心跳周期为10MS，如果该函数返回FALSE则表示掉线
	virtual bool Update();
	//购买商品
	virtual bool CmdBuySomething(pb::Message* message);
	//在线状态
	void SetOnline(bool online) { _stuff.mutable_player_prop()->set_online(online);	}
	bool IsOnline() { return _stuff.player_prop().online(); }
	//签到
	virtual int32_t CmdSign(pb::Message* message);
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
	
	Asset::Inventory_Element* GetMutableInventory(Asset::INVENTORY_TYPE type)
	{
		return _stuff.mutable_inventory()->mutable_inventory(type);
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

	bool IsConnect()
	{
		if (!_session) return false;
			
		return _session->IsConnect();
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
	std::map<int32_t/*麻将牌类型*/, std::vector<int32_t>/*牌值*/> _cards; //玩家手里的牌
	std::map<int32_t/*麻将牌类型*/, std::vector<int32_t>/*牌值*/> _cards_outhand; //玩家墙外牌
	std::vector<Asset::PaiElement> _minggang; //明杠
	std::vector<Asset::PaiElement> _angang; //暗杠
	int32_t _jiangang = 0; //旋风杠，本质是明杠
	int32_t _fenggang = 0; //旋风杠，本质是暗杠
public:
	//玩家操作
	virtual int32_t CmdGameOperate(pb::Message* message); //游戏操作
	virtual int32_t CmdPaiOperate(pb::Message* message); //牌操作
	virtual int32_t CmdGetReward(pb::Message* message); //领取奖励
	virtual int32_t CmdLoadScene(pb::Message* message); //加载场景
	virtual int32_t CmdLuckyPlate(pb::Message* message); //幸运转盘
	virtual int32_t CmdSaizi(pb::Message* message); //打股子
	void OnEnterScene();
	//获取房间
	virtual std::shared_ptr<Room> GetRoom() { return _locate_room; }	//获取当前房间
	virtual void SetRoomID(int64_t room_id) { _stuff.mutable_player_prop()->set_room_id(room_id); }	
	virtual int32_t GetRoomID() { return _stuff.player_prop().room_id(); }
	virtual bool HasRoom() { return _locate_room != nullptr; }

	void SetGame(std::shared_ptr<Game> game) { _game = game; }

	virtual int32_t OnFaPai(std::vector<int32_t>& cards); //发牌

	std::vector<Asset::PAI_OPER_TYPE> CheckPai(const Asset::PaiElement& pai, int64_t from_player_id);

	bool CheckBaoHu(const Asset::PaiElement& pai);

	bool CheckHuPai(const Asset::PaiElement& pai, std::vector<Asset::FAN_TYPE>& fan_list); //胡牌

	bool CheckGangPai(const Asset::PaiElement& pai, int64_t from_player_id); //是否可以杠牌
	bool CheckAllGangPai(std::vector<Asset::PaiElement>& pais); //有玩家一直不杠牌, 每次都要提示, 比如玩家碰了7条,但是手里有7-8-9条,而选择暂时不杠

	void OnGangPai(const Asset::PaiElement& pai, int64_t from_player_id); //杠牌
	
	bool CheckFengGangPai(std::map<int32_t/*麻将牌类型*/, std::vector<int32_t>/*牌值*/>& cards); //是否有旋风杠
	bool CheckJianGangPai(std::map<int32_t/*麻将牌类型*/, std::vector<int32_t>/*牌值*/>& cards); //是否有箭杠
	void OnGangFengPai(); //旋风杠
	void OnGangJianPai(); //箭杠

	bool CheckFengGangPai(); //是否可以旋风杠-风杠 
	bool CheckJianGangPai(); //是否可以旋风杠-箭杠
	
	bool CheckTingPai(); //是否可以听牌：能不能听牌，主要是看是否给牌可以胡

	bool CheckPengPai(const Asset::PaiElement& pai); //是否可以碰牌
	void OnPengPai(const Asset::PaiElement& pai); //碰牌

	bool CheckChiPai(const Asset::PaiElement& pai); //是否可以吃牌
	void OnChiPai(const Asset::PaiElement& pai, pb::Message* message); //吃牌

	bool IsKaimen() { return _cards_outhand.size() != 0 || _minggang.size() != 0; } //是否开门
	bool IsBimen() { return _cards_outhand.size() == 0 && _minggang.size() == 0; } //是否闭门

	bool IsTingPai() { return _stuff.player_prop().has_tinged(); } //是否听牌
	int32_t GetCountAfterTingOperation() { return _stuff.player_prop().oper_count_tingpai(); } //听牌后玩家操作
	void IncreaseTingOperationCount(){ //听牌后操作
		 _stuff.mutable_player_prop()->set_oper_count_tingpai(GetCountAfterTingOperation() + 1);
	}

	//明杠数量
	int32_t GetMingGangCount() {
		return _jiangang + _minggang.size();
	}
	//暗杠数量
	int32_t GetAnGangCount() {
		return _fenggang + _angang.size();
	}
	//是否已经在准备状态 
	bool IsReady() { return _stuff.player_prop().game_oper_state() == Asset::GAME_OPER_TYPE_START; }
	//获取玩家座次
	Asset::POSITION_TYPE GetPosition() { return _stuff.player_prop().position(); }
	void SetPosition(Asset::POSITION_TYPE position) { _stuff.mutable_player_prop()->set_position(position); }

	void PrintPai();
	void ClearCards(); //清理玩家手中牌
	void SynchronizePai();
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
