#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "Asset.h"
#include "Entity.h"

/*
 *
 * 所有实体的最大视野
 *
 * 发送消息侧，不对视野距离进行判断.
 *
 * 对于不对等视野，在接收消息侧进行判断目标到自己是否满足自己视野，不满足则直接退出对消息的处理.
 *
 * 优化的九宫格、十字链表，利用hash表查找玩家P(x0,y0)周围的实体，效率非常高. 如图原理：
 *
 * 	|	      |
 * -------------------------- y2
 * 	|xxxxxxxxxxxxx|
 * 	|xxxxxxxxxxxxx|
 * 	|xxxxxxPxxxxxx|
 * 	|xxxxxxxxxxxxx|
 * 	|xxxxxxxxxxxxx|
 * --------------------------- y1
 * 	|	      |
 * 	x1		x2
 *
 *
 * */
#define MAX_SIGHT_DISTANCE 60

namespace Adoter
{

class Entity;

class Scene : public std::enable_shared_from_this<Scene>
{
private:
	Asset::Scene _stuff;
	std::unordered_map<int32_t, std::unordered_set<int64_t>> _x_entities; //X轴上实体
	std::unordered_map<int32_t, std::unordered_set<int64_t>> _z_entities; //Z轴上实体
public:
	Scene() { }
	Scene(int64_t scene_id);
	Scene(Asset::Scene& scene);
	Scene(Asset::Scene* scene);

	//获取场景ID
	virtual int64_t GetID() { return _stuff.common_prop().global_id(); }
public:
	void EnterScene(std::shared_ptr<Entity> entity);
	void LeaveScene(Entity* entity);
	//更新玩家在场景中的位置
	void UpdatePosition(Entity* entity, const Asset::Vector3& position);
	//获取附近指定类型的实体
	int32_t GetNearByEntities(Entity* entity, std::vector<int64_t>& entities/*包括自己*/); 
};

class SceneManager
{
private:
	std::unordered_map<int64_t, std::shared_ptr<Scene>> _scenes;
public:
	static SceneManager& Instance()
	{
		static SceneManager _instance;
		return _instance;
	}
	
	bool Load()
	{
		auto& scenes = AssetInstance.GetMessagesByType(Asset::ASSET_TYPE_SCENE);
		for (auto s : scenes)
		{
			auto asset_scene = dynamic_cast<Asset::Scene*>(s);
			if (!asset_scene) return false;
			auto scene = std::make_shared<Scene>(asset_scene);
			_scenes.emplace(scene->GetID(), scene);
		}
		return true;
	}

	std::shared_ptr<Scene> Get(int64_t scene_id)
	{
		auto it = _scenes.find(scene_id);
		if (it == _scenes.end()) return nullptr;
		return it->second;
	}
};

#define SceneInstance SceneManager::Instance()

}
