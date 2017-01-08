#include "Scene.h"
#include <vector>
#include <algorithm>

namespace Adoter
{

Scene::Scene(Asset::Scene& scene)
{
	this->_stuff = scene;
}
	
Scene::Scene(Asset::Scene* scene)
{
	this->_stuff = *scene;
}

Scene::Scene(int64_t scene_id) 
{
	//数据初始化
	pb::Message* message = AssetInstance.Get(scene_id);
	if (message)
	{
		Asset::Scene* scene = dynamic_cast<Asset::Scene*>(message);
		if (scene) this->_stuff = *scene;
	}
}

void Scene::EnterScene(std::shared_ptr<Entity> entity)
{
	if (!entity) return; //没有实体

	EntityInstance.Add(entity);

	_x_entities[entity->GetX()].insert(entity->GetID());
	_z_entities[entity->GetZ()].insert(entity->GetID());
}

void Scene::UpdatePosition(Entity* entity, const Asset::Vector3& position)
{
	if (!entity) return; //没有实体

	//std::cout << "=================================" << std::endl;
	//std::cout << __func__ << " of Scene: " << entity->GetID() << " from (" << entity->GetX() << "," << entity->GetZ() << ") to (" 
	//		<< position.x() << "," << position.z() << ")" << std::endl;

	_x_entities[entity->GetX()].erase(entity->GetID());
	_z_entities[entity->GetZ()].erase(entity->GetID());

	_x_entities[position.x()].emplace(entity->GetID());
	_z_entities[position.z()].emplace(entity->GetID());
}

void Scene::LeaveScene(Entity* entity)
{
	if (!entity) return; //没有实体

	_x_entities[entity->GetX()].erase(entity->GetID());
	_z_entities[entity->GetZ()].erase(entity->GetID());
}

int32_t Scene::GetNearByEntities(Entity* entity, std::vector<int64_t>& entities)
{
	if (!entity) return 0; //没有实体
	
	int32_t x = entity->GetX();
	int32_t z = entity->GetZ();
	std::unordered_set<int64_t> x_entities, z_entities;
	for (int32_t xi = x - MAX_SIGHT_DISTANCE; xi <= x + MAX_SIGHT_DISTANCE; ++xi)
	{
		x_entities.insert(_x_entities[xi].begin(), _x_entities[xi].end());
	}
	for (int32_t zi = z - MAX_SIGHT_DISTANCE; zi <= z + MAX_SIGHT_DISTANCE; ++zi)
	{
		z_entities.insert(_z_entities[zi].begin(), _z_entities[zi].end());
	}
	std::set_intersection(x_entities.begin(), x_entities.end(), z_entities.begin(), z_entities.end(), std::back_inserter(entities));

	return entities.size();
}

}
