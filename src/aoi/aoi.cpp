
#include "aoi.h"
#include "GridIdxMgr.h"
#include "log_def.h"
using namespace std;
using namespace aoi;

aoi::Entity::~Entity()
{
	if (nullptr != m_scene)
	{
		L_ERROR("can't del entity when not leave scene"); //还没离开 场景，不允许删除对象。 因为析构函数不能调用 虚函数 OnDelObserver，所以需要用户先离开场景
	}
}

bool aoi::Entity::Enter(Scene &scene, uint16_t x, uint16_t y)
{
	if (x >= MAP_MAX_POS_X || y >= MAP_MAX_POS_Y)
	{
		L_ERROR("x,y is too big %d %d", x, y);
		return false;
	}
	//必须先离开，才能改位置. 保证scene能找到entity
	if (m_scene)
	{
		m_scene->EntityLeave(*this);
	}

	m_gridIdx = (MAP_SCREEN_X * (y / SCREEN_GRID_HEIGHT) + (x / SCREEN_GRID_WIDTH));
	return scene.EntityEnter(*this);
}

bool aoi::Entity::Leave()
{
	if (!m_scene)
	{
		return false;
	}
	return m_scene->EntityLeave(*this);
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
	if (x >= MAP_MAX_POS_X || y >= MAP_MAX_POS_Y)
	{
		L_ERROR("x,y is too big %d %d", x, y);
		return;
	}
	uint16_t oldIdx = m_gridIdx;
	m_gridIdx = (MAP_SCREEN_X * (y / SCREEN_GRID_HEIGHT) + (x / SCREEN_GRID_WIDTH));

	L_ASSERT(m_gridIdx < MAP_SCREEN_MAX);
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
		f(*p);//天知道里面做什么！所以冻结住才安全
	}
	m_scene->Freeze(false);
}

aoi::Scene::~Scene()
{
	for (auto &v : m_idx2VecEntity)
	{
		for (Entity *entity : v.second)
		{
			entity->OnSceneDel();
		}
	}
}

size_t aoi::Scene::GetEntityNum()
{
	size_t num = 0;
	for (auto &v : m_idx2VecEntity)
	{
		num += v.second.size();
	}
	return num;
}

bool aoi::Scene::EntityEnter(Entity &entity)
{
	L_COND(!m_isFreeze, false);
	m_isFreeze = true;
	uint16_t gridIdx = entity.GridIdx();

	const VecGridIdx &ninescreen = GridIdxMgr::Ins().Get9Grid(gridIdx);
	for ( const uint16_t &v : ninescreen)
	{
		for (Entity *otherEntity : m_idx2VecEntity[v])
		{
			L_ASSERT(otherEntity != &entity);
			otherEntity->AddObserver(entity);
			entity.AddObserver(*otherEntity);
		}
	}
	m_idx2VecEntity[gridIdx].push_back(&entity);
	entity.SetScene(this);
	m_isFreeze = false;
	return true;
}

bool aoi::Scene::EntityLeave(Entity &entity)
{
	L_COND(!m_isFreeze, false);
	if (!entity.GetScene())
	{
		return false;
	}

	m_isFreeze = true;
	uint16_t gridIdx = entity.GridIdx();

	VecEntity &vecEntity = m_idx2VecEntity[gridIdx];
	SimpleRemoveFromVec(vecEntity, &entity);
	entity.SetScene(nullptr);

	const VecGridIdx &ninescreen = GridIdxMgr::Ins().Get9Grid(gridIdx);
	for (const uint16_t &v : ninescreen)
	{
		for (Entity *otherEntity : m_idx2VecEntity[v])
		{
			L_ASSERT(otherEntity != &entity);
			otherEntity->DelObserver(entity);
			entity.DelObserver(*otherEntity);
		}
	}

	m_isFreeze = false;
	return true;
}

bool aoi::Scene::UpdateEntity(Entity &entity, uint16_t oldGridIdx, uint16_t newGridIdx)
{
	L_ASSERT(oldGridIdx != newGridIdx);
	L_COND(!m_isFreeze, false);
	L_COND(entity.GetScene(), false);
	m_isFreeze = true;
	if (GridIdxMgr::Ins().checkTwoPosIInNine(oldGridIdx, newGridIdx))
	{
		uint8_t dir = GridIdxMgr::Ins().getScreenDirect(oldGridIdx, newGridIdx);
		{
			SimpleRemoveFromVec(m_idx2VecEntity[oldGridIdx], &entity);
			const VecGridIdx &vecGridIdx = GridIdxMgr::Ins().getReverseDirectScreen(oldGridIdx, dir);
			for (uint16_t v : vecGridIdx)
			{
				for (Entity *otherEntity : m_idx2VecEntity[v])
				{
					L_ASSERT(otherEntity != &entity);
					otherEntity->DelObserver(entity);
					entity.DelObserver(*otherEntity);
				}
			}
		}
		{
			const VecGridIdx &vecGridIdx = GridIdxMgr::Ins().getDirectScreen(newGridIdx, dir);
			for (uint16_t v : vecGridIdx)
			{
				for (Entity *otherEntity : m_idx2VecEntity[v])
				{
					L_ASSERT(otherEntity != &entity);
					otherEntity->AddObserver(entity);
					entity.AddObserver(*otherEntity);
				}
			}
			m_idx2VecEntity[newGridIdx].push_back(&entity);
		}
	}
	else
	{
		{
			SimpleRemoveFromVec(m_idx2VecEntity[oldGridIdx], &entity);
			const VecGridIdx &vecGridIdx = GridIdxMgr::Ins().Get9Grid(oldGridIdx);
			for (uint16_t v : vecGridIdx)
			{
				for (Entity *otherEntity : m_idx2VecEntity[v])
				{
					L_ASSERT(otherEntity != &entity);
					otherEntity->DelObserver(entity);
					entity.DelObserver(*otherEntity);
				}
			}
		}
		{
			//L_DEBUG("%p newGridIdx=%d", &entity, newGridIdx);
			const VecGridIdx &vecGridIdx = GridIdxMgr::Ins().Get9Grid(newGridIdx);
			//L_DEBUG("9 grid size=%d", vecGridIdx.size());
			for (uint16_t v : vecGridIdx)
			{
				for (Entity *otherEntity : m_idx2VecEntity[v])
				{
					//L_DEBUG("see eachother =%p %p", &entity, otherEntity);
					L_ASSERT(otherEntity != &entity);
					otherEntity->AddObserver(entity);
					entity.AddObserver(*otherEntity);
				}
			}
			m_idx2VecEntity[newGridIdx].push_back(&entity);
		}
	}

	m_isFreeze = false;
	return true;
}

