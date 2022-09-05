
#include "aoi.h"
#include "GridIdxMgr.h"
#include "log_def.h"
using namespace std;
using namespace aoi;

namespace
{
	namespace
	{
		template<typename T, std::size_t N>
		constexpr std::size_t ArrayLen(T(&)[N])
		{ // constexpr
			return N; // and
		}
	}

	using PEntity = Entity*;
	struct PriList
	{
		uint32_t num = 0;
		PEntity entityList[Entity::MAX_SEE_PLAYER];
	};

	class Cfg 
	{
	public:
		bool emitterCfg[(uint32_t)EntityType::Max][(uint32_t)EntityType::Max] = {}; // emitterCfg[type1][typ2] == true 表示 type1 需要发射光线到 type2 

	public:
		static Cfg& Ins()
		{
			static Cfg cfg;
			return cfg;
		}

	private:
		Cfg()
		{
			emitterCfg[(uint32_t)EntityType::Player][(uint32_t)EntityType::Player] = true;
			emitterCfg[(uint32_t)EntityType::Player][(uint32_t)EntityType::Npc]    = false;
			emitterCfg[(uint32_t)EntityType::Player][(uint32_t)EntityType::Item]   = false;

			emitterCfg[(uint32_t)EntityType::Npc][(uint32_t)EntityType::Player]    = true;
			emitterCfg[(uint32_t)EntityType::Npc][(uint32_t)EntityType::Npc]       = false;
			emitterCfg[(uint32_t)EntityType::Npc][(uint32_t)EntityType::Item]      = false;

			emitterCfg[(uint32_t)EntityType::Item][(uint32_t)EntityType::Player]   = true;
			emitterCfg[(uint32_t)EntityType::Item][(uint32_t)EntityType::Npc]      = false;
			emitterCfg[(uint32_t)EntityType::Item][(uint32_t)EntityType::Item]     = false;
		}
	};
}


aoi::Entity::~Entity()
{
	if (nullptr != m_scene)
	{
		L_ERROR("can't del entity when not leave scene"); //还没离开 场景，不允许删除对象。 
		//m_scene->EntityLeave(*this);//不能调用看离开场景。 因为析构函数不能调用 虚函数 OnDelObserver，所以需要用户先离开场景
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

//void ErrorFun()
//{
//
//}
void aoi::Entity::AddObserver(Entity &other)
{
	//{//check error code
	//	if (other.m_type == EntityType::Player)
	//	{
	//		if (other.m_seePlayerNum >= (uint32_t)Entity::MAX_SEE_PLAYER)
	//		{
	//			ErrorFun();
	//		}
	//		L_ASSERT(other.m_seePlayerNum < (uint32_t)Entity::MAX_SEE_PLAYER, other.m_seePlayerNum, (uint32_t)Entity::MAX_SEE_PLAYER);
	//	}
	//}

	L_ASSERT(!m_isFreeze);
	if (!m_playerObservers.insert(&other).second)
	{
		return;
	}
	other.m_seePlayerNum++;
	//LDEBUG("AddObserver other.m_seePlayerNum=", other.m_seePlayerNum, "other=", &other, "this=", this);
	OnAddObserver(other);
}


void aoi::Entity::TryDelObserver(Entity &other)
{
	L_ASSERT(!m_isFreeze);
	if (1 != m_playerObservers.erase(&other))
	{
		return;
	}
	other.m_seePlayerNum--;
	L_ASSERT(other.m_seePlayerNum >= 0);
	//LDEBUG("dec seenum", &other, other.m_seePlayerNum);
	OnDelObserver(other);
}

void aoi::Entity::TryAddObserver(Entity& other)
{
	L_ASSERT(other.m_type == EntityType::Player && m_type == EntityType::Player); //非法调用，只有player之间才需要调用
	if (other.m_seePlayerNum >= (uint32_t)Entity::MAX_SEE_PLAYER)
	{//不做优先替换处理了。简化 
		return; 
	}
	AddObserver(other);
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
	m_scene->Freeze(true);//不让scene改变状态，因为scene 改变会又机会改变 entity 状态
	m_isFreeze = true;
	for (Entity *p : m_playerObservers)
	{
		f(*p);//天知道里面做什么！所以冻结住才安全
	}
	m_isFreeze = false;
	m_scene->Freeze(false);
}

aoi::Scene::~Scene()
{
	VecEntity vec;
	for (auto& i : m_idx2VecEntityArray)
	{
		for (VecEntity& v : i.second)
		{
			vec.insert(vec.end(), v.begin(), v.end());
		}
	}
	for (Entity *entity : vec)
	{
		EntityLeave(*entity);
	}
}

size_t aoi::Scene::GetEntityNum()
{
	size_t num = 0;
	for (auto& i : m_idx2VecEntityArray)
	{
		for (VecEntity& v : i.second)
		{
			num += v.size();
		}
	}
	return num;
}

//player 之间 可视数量裁剪
//@entity 更新视野玩家
//@vecAllPlayer entity视野内其他玩家
void aoi::Scene::CalPlayerObserver(Entity& entity, const VecEntity &vecAllPlayer)
{
	//初始化所有视野玩家优先列表
	PriList priList[(uint32_t)ViewPriolity::Max];    //优先列表，0优先级最大
	for (Entity* otherPlayer : vecAllPlayer)
	{
		uint32_t idx = (uint32_t)entity.GetViewPriolity(*otherPlayer);
		L_ASSERT(idx < (uint32_t)ViewPriolity::Max);
		PriList& d = priList[idx];
		if (d.num >= ArrayLen(d.entityList))
		{
			continue;
		}
		d.entityList[d.num] = otherPlayer;
		d.num++;
	}

	//初始化entity 已见玩家 集合	
	std::vector< Entity*> vecOldSeeOther;
	for (Entity* otherPlayer : vecAllPlayer)
	{
		if (otherPlayer->m_playerObservers.find(&entity) != otherPlayer->m_playerObservers.end())
		{
			vecOldSeeOther.push_back(otherPlayer);
		}
	}

	//根据优先级，增加player之间视野
	std::vector< Entity*> vecNewSeeOther;//新计算的 已见玩家 集合	
	uint32_t addNum = 0;
	for (PriList& d : priList)
	{
		for (uint32_t i = 0; i < d.num; i++)
		{
			vecNewSeeOther.push_back(d.entityList[i]);
			addNum++;
			if (addNum >= Entity::MAX_SEE_PLAYER)
			{
				break;
			}
		}
		if (addNum >= Entity::MAX_SEE_PLAYER)
		{
			break;
		}
	}

	//vecOldSeeOther vecNewSeeOther 差集， 执行可见变不可见
	//交集，不用变化
	for (auto& old : vecOldSeeOther)
	{
		bool bFind = false;
		for (auto& newEntity : vecNewSeeOther)
		{
			if (old == newEntity)
			{
				bFind = true;
			}
		}
		if (!bFind)
		{
			old->TryDelObserver(entity);
		}
	}
	for (auto& newEntity : vecNewSeeOther)
	{
		bool bFind = false;
		for (auto& old : vecOldSeeOther)
		{
			if (old == newEntity)
			{
				bFind = true;
			}
		}
		if (!bFind)
		{
			newEntity->AddObserver(entity);
		}
	}
}

bool aoi::Scene::EntityEnter(Entity &entity)
{
	L_ASSERT((uint32_t)entity.m_type < (uint32_t)EntityType::Max);
	L_COND(nullptr == entity.GetScene(), false);
	L_COND(!m_isFreeze, false);//迭代m_observers 期间禁止调用， 用户设法延时调用吧。
	m_isFreeze = true;
	uint16_t gridIdx = entity.GridIdx();

	const VecGridIdx &ninescreen = GridIdxMgr::Ins().Get9Grid(gridIdx);
	VecEntity vecAllPlayer;
	//LDEBUG(&entity, "enter");
	for ( const uint16_t &v : ninescreen)
	{
		for (uint32_t i=0; i< (uint32_t)EntityType::Max; ++i)
		{
			const VecEntity& vec = m_idx2VecEntityArray[v][i];
			if (i == (uint32_t)EntityType::Player && entity.m_type == EntityType::Player)
			{//player之间先记录，后续再处理
				for (Entity* otherEntity : vec)
				{
					//LDEBUG("add vecAllPlayer", otherEntity);
					vecAllPlayer.push_back(otherEntity);
					entity.TryAddObserver(*otherEntity);
				}
			}
			else
			{
				for (Entity* otherEntity : vec)
				{
					L_ASSERT(otherEntity != &entity);
					auto& emitterCfg = Cfg::Ins().emitterCfg;
					if (emitterCfg[(uint32_t)otherEntity->m_type][(uint32_t)entity.m_type])
					{
						otherEntity->AddObserver(entity);
					}
					if (emitterCfg[(uint32_t)entity.m_type][(uint32_t)otherEntity->m_type])
					{
						entity.AddObserver(*otherEntity);
					}
				}
			}
		}
	}
	if (vecAllPlayer.size() <= Entity::MAX_SEE_PLAYER)//大概率不超，快速执行全部加
	{
		//{//tmp code
		//	if (!vecAllPlayer.empty())
		//	{
		//		LDEBUG("start easy add");
		//	}
		//}
		for (auto i : vecAllPlayer)
		{
		//	LDEBUG(i, "AddObserver", &entity);
			i->AddObserver(entity);
		}
	}
	else
	{
		CalPlayerObserver(entity, vecAllPlayer);
	}
	m_idx2VecEntityArray[gridIdx][(uint32_t)entity.m_type].push_back(&entity);
	entity.SetScene(this);
	m_isFreeze = false;
	return true;
}

bool aoi::Scene::EntityLeave(Entity &entity)
{
	//LDEBUG("EntityLeave");
	L_COND(!m_isFreeze, false); //迭代m_observers 期间禁止调用， 用户设法延时调用吧。
	if (!entity.GetScene())
	{
		return false;
	}

	m_isFreeze = true;
	uint16_t gridIdx = entity.GridIdx();

	VecEntity &vecEntity = m_idx2VecEntityArray[gridIdx][(uint32_t)entity.m_type];
	SimpleRemoveFromVec(vecEntity, &entity);
	entity.SetScene(nullptr);

	const VecGridIdx &ninescreen = GridIdxMgr::Ins().Get9Grid(gridIdx);
	for (const uint16_t &v : ninescreen)
	{
		for (VecEntity& vec : m_idx2VecEntityArray[v])
		{
			for (Entity* otherEntity : vec)
			{
				L_ASSERT(otherEntity != &entity);
				otherEntity->TryDelObserver(entity);
				entity.TryDelObserver(*otherEntity);
				//LDEBUG("otherEntity->TryDelObserver");
			}
		}
	}
	L_ASSERT(entity.m_playerObservers.empty());
	m_isFreeze = false;
	return true;
}

bool aoi::Scene::UpdateEntity(Entity &entity, uint16_t oldGridIdx, uint16_t newGridIdx)
{
	L_ASSERT(oldGridIdx != newGridIdx);
	L_COND(!m_isFreeze, false);//迭代m_observers 期间禁止调用， 用户设法延时调用吧。
	L_COND(entity.GetScene(), false);
	m_isFreeze = true;
	if (GridIdxMgr::Ins().checkTwoPosIInNine(oldGridIdx, newGridIdx))
	{
	//	LDEBUG(&entity, "update in nine");
		uint8_t dir = GridIdxMgr::Ins().getScreenDirect(oldGridIdx, newGridIdx);
		{//互相删旧区域，现在变不可见视野
			SimpleRemoveFromVec(m_idx2VecEntityArray[oldGridIdx][(uint32_t)entity.m_type], &entity);
			const VecGridIdx &vecGridIdx = GridIdxMgr::Ins().getReverseDirectScreen(oldGridIdx, dir);
			for (uint16_t v : vecGridIdx)
			{
				for (VecEntity& vec : m_idx2VecEntityArray[v])
				{
					for (Entity* otherEntity : vec)
					{
						L_ASSERT(otherEntity != &entity);
						otherEntity->TryDelObserver(entity);
						entity.TryDelObserver(*otherEntity);
						//LDEBUG("del each", otherEntity, &entity);
					}
				}
			}
		}
		{//处理新视野的区域
			const VecGridIdx& vecGridIdx = GridIdxMgr::Ins().getDirectScreen(newGridIdx, dir);
			for (uint16_t v : vecGridIdx)
			{
				for (uint32_t i = 0; i < (uint32_t)EntityType::Max; ++i)
				{
					const VecEntity& vec = m_idx2VecEntityArray[v][i];
					if (i == (uint32_t)EntityType::Player && entity.m_type == EntityType::Player)
					{
						for (Entity* otherEntity : vec)
						{
							L_ASSERT(otherEntity != &entity);
							//otherEntity.TryAddObserver(*entity); //entity视野延后裁剪处理
							entity.TryAddObserver(*otherEntity);
						}
					}
					else
					{
						for (Entity* otherEntity : vec)
						{
							L_ASSERT(otherEntity != &entity);
							auto& emitterCfg = Cfg::Ins().emitterCfg;
							if (emitterCfg[(uint32_t)otherEntity->m_type][(uint32_t)entity.m_type])
							{
								otherEntity->AddObserver(entity);
							}
							if (emitterCfg[(uint32_t)entity.m_type][(uint32_t)otherEntity->m_type])
							{
								entity.AddObserver(*otherEntity);
							}
						}
					}
				}
			}

			m_idx2VecEntityArray[newGridIdx][(uint32_t)entity.m_type].push_back(&entity);
		}

		if (entity.m_type == EntityType::Player)
		{
			VecEntity vecAllPlayer;
			for (uint16_t v : GridIdxMgr::Ins().Get9Grid(newGridIdx))
			{
				const VecEntity& vec = m_idx2VecEntityArray[v][(uint32_t)EntityType::Player];
				for (Entity* otherEntity : vec)
				{
					if (otherEntity != &entity)
					{
					//	LDEBUG("vecAllPlayer.push_back", otherEntity);
						vecAllPlayer.push_back(otherEntity);
					}
				}
			}
			if (vecAllPlayer.size() <= Entity::MAX_SEE_PLAYER)//大概率不超，快速执行全部加
			{
				for (auto i : vecAllPlayer)
				{
				//	LDEBUG(i, "AddObserver", &entity);
					i->AddObserver(entity);
				}
			}
			else
			{
				CalPlayerObserver(entity, vecAllPlayer);
			}
		}
	}
	else
	{
		//LDEBUG(&entity, "update not in nine");
		{//互相删旧区域，现在变不可见视野
			SimpleRemoveFromVec(m_idx2VecEntityArray[oldGridIdx][(uint32_t)entity.m_type], &entity);
			for (uint16_t v : GridIdxMgr::Ins().Get9Grid(oldGridIdx))
			{
				for (VecEntity& vec : m_idx2VecEntityArray[v])
				{
					for (Entity* otherEntity : vec)
					{
						L_ASSERT(otherEntity != &entity);
						otherEntity->TryDelObserver(entity);
						entity.TryDelObserver(*otherEntity);
					}
				}
			}
		}
		{
			//L_DEBUG("%p newGridIdx=%d", &entity, newGridIdx);
			VecEntity vecAllPlayer;
			for (uint16_t v : GridIdxMgr::Ins().Get9Grid(newGridIdx))
			{
				for (uint32_t i = 0; i < (uint32_t)EntityType::Max; ++i)
				{
					const VecEntity& vec = m_idx2VecEntityArray[v][i];
					if (i == (uint32_t)EntityType::Player && entity.m_type == EntityType::Player)
					{//player之间先记录，后续再处理
						for (Entity* otherEntity : vec)
						{
							vecAllPlayer.push_back(otherEntity);
							entity.TryAddObserver(*otherEntity);
						}
					}
					else
					{
						for (Entity* otherEntity : vec)
						{
							//L_DEBUG("see eachother =%p %p", &entity, otherEntity);
							L_ASSERT(otherEntity != &entity);
							auto& emitterCfg = Cfg::Ins().emitterCfg;
							if (emitterCfg[(uint32_t)otherEntity->m_type][(uint32_t)entity.m_type])
							{
								otherEntity->AddObserver(entity);
							}
							if (emitterCfg[(uint32_t)entity.m_type][(uint32_t)otherEntity->m_type])
							{
								entity.AddObserver(*otherEntity);
							}
						}
					}
				}
			}

			if (vecAllPlayer.size() <= Entity::MAX_SEE_PLAYER)//大概率不超，快速执行全部加
			{
				for (auto& i : vecAllPlayer)
				{
					i->AddObserver(entity);
				}
			}
			else
			{
				CalPlayerObserver(entity, vecAllPlayer);
			}

			m_idx2VecEntityArray[newGridIdx][(uint32_t)entity.m_type].push_back(&entity);
		}
	}

	m_isFreeze = false;
	return true;
}

