
#include "Scene.h"
#include "screen_index.h"

using namespace std;
using namespace aoi;

aoi::Scene::~Scene()
{

	//all entiry , entity.Scene(nullptr);
}

bool aoi::Scene::EntityEnter(Entity &entity)
{
	if (entity.GetScene())
	{
		L_ERROR("repeated EntityEnter");
		return false;
	}

	uint16_t gridIdx = entity.GridIdx();

	const VecGridIdx &ninescreen = ScreenIndexBase::Ins().getNineScreen(gridIdx);
	for ( const uint16_t &v : ninescreen)
	{
		for (Entity *otherEntity : m_posi2vecEntity[gridIdx])
		{
			otherEntity->AddObserver(entity);
			entity.AddObserver(*otherEntity);
		}
	}

	m_posi2vecEntity[gridIdx].push_back(&entity);
	entity.SetScene(this);
	return true;
}

bool aoi::Scene::EntityLeave(Entity &entity)
{
	if (!entity.GetScene())
	{
		L_ERROR("entity not in scene");
		return false;
	}

	uint16_t gridIdx = entity.GridIdx();

	const VecGridIdx &ninescreen = ScreenIndexBase::Ins().getNineScreen(gridIdx);
	for (const uint16_t &v : ninescreen)
	{
		for (Entity *otherEntity : m_posi2vecEntity[gridIdx])
		{
			otherEntity->DelObserver(entity);
			entity.DelObserver(*otherEntity);
		}
	}

	VecEntity &vecEntity = m_posi2vecEntity[gridIdx];
	SimpleRemoveFromVec(vecEntity, &entity);
	entity.SetScene(nullptr);
	return true;
}

bool aoi::Scene::UpdateEntity(Entity &entity, uint16_t oldGridIdx, uint16_t newGridIdx)
{
	L_ASSERT(oldGridIdx != newGridIdx);

	return true;
}

aoi::Entity::~Entity()
{
	if (nullptr != m_scene)
	{
		m_scene->EntityLeave(*this);
	}
}

void aoi::Entity::AddObserver(Entity &entity)
{
	m_observers.insert(&entity);
	OnAddObserver(entity);
}

void aoi::Entity::DelObserver(Entity &entity)
{
	m_observers.erase(&entity);
	OnDelObserver(entity);
}

void aoi::Entity::UpdatePos(uint16_t x, uint16_t y)
{
	uint16_t oldIdx = m_gridIdx;
	m_gridIdx = (MAP_SCREEN_X * (y / SCREEN_GRID_HEIGHT) + (x / SCREEN_GRID_WIDTH));
	if (oldIdx == m_gridIdx)
	{
		return;
	}
	if (m_scene)
	{
		m_scene->UpdateEntity(*this, oldIdx, m_gridIdx);
	}
}
