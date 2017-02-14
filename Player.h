#pragma once

#include <mutex>
#include <cmath>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include "Player.h"
#include "P_Header.h"
#include "WorldSession.h"
#include "MessageDispatcher.h"
#include "Item.h"
#include "Asset.h"
#include "Room.h"

namespace Adoter
{
namespace pb = google::protobuf;

class Room;

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
	const Asset::Player& Get() { return _stuff; }
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
	virtual void SendResponse(pb::Message* message);
	virtual void SendToRoomers(pb::Message& message); //向房间里玩家发送协议数据，发送到客户端
	virtual void BroadCast(Asset::MsgItem& item);
	//进入游戏
	virtual int32_t CmdEnterGame(pb::Message* message);
	virtual int32_t OnEnterGame();
	//创建房间
	virtual int32_t CmdCreateRoom(pb::Message* message);
	virtual void OnCreateRoom(Asset::CreateRoom* message);
	//进入房间
	virtual int32_t CmdEnterRoom(pb::Message* message);
	virtual void OnEnterRoom(int64_t room_id = 0);
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
public:
	//获取所有包裹
	const Asset::Inventories& GetInventories()
	{
		return _stuff.inventory();
	}

	//获取指定包裹
	const Asset::Inventories_Inventory& GetInventory(Asset::INVENTORY_TYPE type)
	{
		return _stuff.inventory().inventories(type);
	}	
	
	//获取物品
	bool GainItem(Item* item);
	bool GainItem(int64_t global_item_id);
	//存放物品
	bool PushBackItem(Asset::INVENTORY_TYPE inventory_type, Item* item);

	const std::shared_ptr<WorldSession> GetSession()
	{
		return _session;
	}
	//发送错误信息
	void AlterError(Asset::ERROR_CODE error_code, Asset::ERROR_TYPE error_type = Asset::ERROR_TYPE_NORMAL, Asset::ERROR_SHOW_TYPE error_show_type = Asset::ERROR_SHOW_TYPE_CHAT);

///////游戏逻辑定义
private:
	std::shared_ptr<Room> _locate_room = nullptr; //实体所在房间
	std::unordered_map<int/*麻将牌类型*/, std::vector<int>/*牌值*/> _cards; //玩家具有的麻将
public:
	//获取房间
	virtual std::shared_ptr<Room>& GetRoom() { return _locate_room; }	//获取当前房间
	virtual void SetRoomID(int64_t room_id) { _stuff.mutable_common_prop()->set_room_id(room_id); }	
	virtual int32_t GetRoomID() { return this->_stuff.common_prop().room_id(); }
	virtual bool HasRoom() { return _locate_room != nullptr; }

	virtual int32_t OnFaPai(int32_t card); //每次发牌
	virtual int32_t OnFaPai(std::vector<int32_t>& cards); //游戏开始之初发牌
	int ZhuaPai(); //抓牌
	int ChiPai(); //吃牌
	int PengPai(); //碰牌
	int GangPai(); //杠牌
	int HuPai(); //胡牌
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
