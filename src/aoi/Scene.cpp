
#include "Scene.h"
#include "GridIdxMgr.h"
#include "log_def.h"
using namespace std;
using namespace aoi;

aoi::Scene::~Scene()
{
	
	for (auto &v : m_idx2VecEntity)
	{
		VecEntity &vec = v.second;
		for (Entity *entity : vec)
		{
			entity->OnSceneDel();
		}
	}
}

bool aoi::Scene::EntityEnter(Entity &entity)
{
	L_COND(!m_isFreeze, false);
	if (Scene *scene = entity.GetScene())
	{
		scene->EntityLeave(entity);
	}
	uint16_t gridIdx = entity.GridIdx();

	const VecGridIdx &ninescreen = ScreenIndexBase::Ins().getNineScreen(gridIdx);
	for ( const uint16_t &v : ninescreen)
	{
		for (Entity *otherEntity : m_idx2VecEntity[gridIdx])
		{
			otherEntity->AddObserver(entity);
			entity.AddObserver(*otherEntity);
		}
	}

	m_idx2VecEntity[gridIdx].push_back(&entity);
	entity.SetScene(this);
	return true;
}

bool aoi::Scene::EntityLeave(Entity &entity)
{
	L_COND(!m_isFreeze, false);
	if (!entity.GetScene())
	{
		return false;
	}

	uint16_t gridIdx = entity.GridIdx();

	const VecGridIdx &ninescreen = ScreenIndexBase::Ins().getNineScreen(gridIdx);
	for (const uint16_t &v : ninescreen)
	{
		for (Entity *otherEntity : m_idx2VecEntity[gridIdx])
		{
			otherEntity->DelObserver(entity);
			entity.DelObserver(*otherEntity);
		}
	}

	VecEntity &vecEntity = m_idx2VecEntity[gridIdx];
	SimpleRemoveFromVec(vecEntity, &entity);
	entity.SetScene(nullptr);
	return true;
}

bool aoi::Scene::UpdateEntity(Entity &entity, uint16_t oldGridIdx, uint16_t newGridIdx)
{
	L_ASSERT(oldGridIdx != newGridIdx);
	L_COND(!m_isFreeze, false);
	L_COND(entity.GetScene(), false);
	 
	if (ScreenIndexBase::Ins().checkTwoPosIInNine(oldGridIdx, newGridIdx))
	{
		uint8_t dir = ScreenIndexBase::Ins().getScreenDirect(oldGridIdx, newGridIdx);
		{
			const VecGridIdx &vecGridIdx = ScreenIndexBase::Ins().getReverseDirectScreen(oldGridIdx, dir);
			for (uint16_t gridIdx : vecGridIdx)			{				for (Entity *otherEntity : m_idx2VecEntity[gridIdx])				{					otherEntity->DelObserver(entity);					entity.DelObserver(*otherEntity);				}			}
		}
		{
			const VecGridIdx &vecGridIdx = ScreenIndexBase::Ins().getDirectScreen(newGridIdx, dir);
			for (uint16_t gridIdx : vecGridIdx)			{				for (Entity *otherEntity : m_idx2VecEntity[gridIdx])				{					otherEntity->AddObserver(entity);
					entity.AddObserver(*otherEntity);				}			}
		}
	}
	else
	{
		{
			const VecGridIdx &vecGridIdx = ScreenIndexBase::Ins().getNineScreen(oldGridIdx);
			for (uint16_t gridIdx : vecGridIdx)			{				for (Entity *otherEntity : m_idx2VecEntity[gridIdx])				{					otherEntity->DelObserver(entity);					entity.DelObserver(*otherEntity);				}			}
		}
		{
			const VecGridIdx &vecGridIdx = ScreenIndexBase::Ins().getNineScreen(newGridIdx);
			for (uint16_t gridIdx : vecGridIdx)			{				for (Entity *otherEntity : m_idx2VecEntity[gridIdx])				{					otherEntity->AddObserver(entity);
					entity.AddObserver(*otherEntity);				}			}
		}
	}
	return true;
}



aoi::Entity::~Entity()
{
	if (nullptr != m_scene)
	{
		m_scene->EntityLeave(*this);
	}
}



bool aoi::Entity::Enter(Scene &scene, uint16_t x, uint16_t y)
{
	m_gridIdx = (MAP_SCREEN_X * (y / SCREEN_GRID_HEIGHT) + (x / SCREEN_GRID_WIDTH));
	return scene.EntityEnter(*this);
}

bool aoi::Entity::Leave(Scene &scene)
{
	return scene.EntityLeave(*this);
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

void aoi::Entity::OnSceneDel()
{
	m_scene = nullptr;
	m_observers.clear();
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

void aoi::Entity::ForEachObservers(std::function<void(Entity&)> f)
{
	if (!m_scene)
	{
		return;
	}
	m_scene->Freeze(true);
	for (Entity *p : m_observers)
	{
		f(*p);
	}
	m_scene->Freeze(false);
}

