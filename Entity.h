#pragma once

#include <memory>
#include <mutex>
#include <cmath>
#include <unordered_map>
#include "P_Header.h"
#include "Scene.h"

namespace Adoter
{

/*
 * 类说明:
 *
 * 游戏中所有实体的基类
 *
 * 说明：有的游戏中也被称为Object、Actor、Creature...
 *
 * */
namespace pb = google::protobuf;

class Scene;

class Entity : public std::enable_shared_from_this<Entity>
{
public:
	int64_t _ID = 0; //所有实体的唯一识别不能重复
	std::shared_ptr<Scene> _locate_scene = nullptr; //实体所在场景
	Asset::CommonProp _super_common_prop; //公共属性
public:
	Entity(); 
	virtual ~Entity(); 
//////////////////////////////////////属性接口
	//实体ID	
	virtual int64_t GetID() { return this->_ID; }
	virtual void SetID(int64_t entity_id) { this->_ID = entity_id; this->_super_common_prop.set_entity_id(entity_id); } //默认以公共属性中的全局唯一ID作为ID，玩家必须重载.
	//获取级别
	virtual int32_t GetLevel() { return this->_super_common_prop.level(); }
	//获取性别
	virtual int32_t GetGender() { return this->_super_common_prop.gender(); }
	//是否在某个状态
	virtual bool HasAction(const Asset::ENTITY_ACTION_STATE_TYPE type) { return this->_super_common_prop.action_state_type() | type; }
	virtual bool ClearAction(const Asset::ENTITY_ACTION_STATE_TYPE type) { return this->_super_common_prop.action_state_type() | ~type; }
	//是否为玩家
	virtual bool IsPlayer() { return this->_super_common_prop.entity_type() == Asset::ENTITY_TYPE_PLAYER; }
	//设置实体类型
	virtual void SetEntityType(const Asset::ENTITY_TYPE entity_type) { this->_super_common_prop.set_entity_type(entity_type); }
	//获取公共属性
	virtual Asset::CommonProp& GetSuperCommonProp() { return this->_super_common_prop; }
//////////////////////////////////////操作及动作接口
	//出生
	virtual void OnBirth(int64_t scene_id = 0);
	//死亡
	virtual void OnDeath();
	//获取实体最大视野范围
	virtual float GetMaxVisibalRange() { return _super_common_prop.visibility_range(); } //配置
	//获取当前视野
	virtual float GetSightDistance() { return 0.0f; }
	//是否在当前视野
	virtual bool IsInSight(const Asset::Vector3& pos);
	//移动位置
	virtual void StepMove(const Asset::Vector3& position); 
	virtual void MoveTo(const Asset::Vector3& des_pos) { }  //寻路
	//进入场景
	virtual void OnEnterScene(int64_t scene_id = 0);
	//离开场景
	virtual void OnLeaveScene();
	//获取坐标
	virtual float GetX() { return _super_common_prop.position().x(); }
	virtual float GetY() { return _super_common_prop.position().y(); }
	virtual float GetZ() { return _super_common_prop.position().z(); }
	//更新位置
	virtual void UpdatePosition(const Asset::Vector3& position) { _super_common_prop.mutable_position()->CopyFrom(position); }
	//获取场景
	virtual std::shared_ptr<Scene>& GetScene() { return _locate_scene; }	//获取当前场景
	virtual void SetSceneID(int64_t scene_id) { _super_common_prop.set_scene_id(scene_id); }	
	virtual int32_t GetSceneID() { return this->_super_common_prop.scene_id(); }
	virtual bool HasScene() { return _locate_scene != nullptr; }
	//加载数据	
	virtual int32_t Load() { return 0; }
	//保存数据
	virtual int32_t Save() { return 0; }
	//消息处理
	virtual bool HandleMessage(const Asset::MsgItem& item); 
	virtual void SendMessage(Asset::MsgItem& item);
	virtual void SendNearbyMessage(Asset::MsgItem& item); //向周围玩家发送消息数据       
	virtual void BroadCastCommonProp(Asset::MSG_TYPE type); //向周围玩家发送公共数据       
	//协议处理(Protocol Buffer)
	virtual bool HandleProtocol(int32_t type_t, pb::Message* message) { return true; }
	virtual void SendProtocol(pb::Message& message) { }
	virtual void SendNearbyProtocol(pb::Message& message) { } //向周围玩家发送协议数据，发送到客户端
};

class EntityManager : public std::enable_shared_from_this<EntityManager>
{
private:
	std::mutex _mutex;
	std::unordered_map<int64_t, std::shared_ptr<Entity>> _entities; //实体为智能指针，不要传入引用
public:
	static EntityManager& Instance()
	{
		static EntityManager _instance;
		return _instance;
	}

	void Erase(int64_t entity_id);
	void Erase(Entity& entity);
	void Add(std::shared_ptr<Entity> entity);
	void Emplace(int64_t entity_id, std::shared_ptr<Entity> entity);
	bool Has(int64_t entity_id);
	std::shared_ptr<Entity> GetEntity(int64_t id);
};

#define EntityInstance EntityManager::Instance()

}
