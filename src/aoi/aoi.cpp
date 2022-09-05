
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
		bool emitterCfg[(uint32_t)EntityType::Max][(uint32_t)EntityType::Max] = {}; // emitterCfg[type1][typ2] == true ��ʾ type1 ��Ҫ������ߵ� type2 

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
		L_ERROR("can't del entity when not leave scene"); //��û�뿪 ������������ɾ������ 
		//m_scene->EntityLeave(*this);//���ܵ��ÿ��뿪������ ��Ϊ�����������ܵ��� �麯�� OnDelObserver��������Ҫ�û����뿪����
	}
}

bool aoi::Entity::Enter(Scene &scene, uint16_t x, uint16_t y)
{
	if (x >= MAP_MAX_POS_X || y >= MAP_MAX_POS_Y)
	{
		L_ERROR("x,y is too big %d %d", x, y);
		return false;
	}
	//�������뿪�����ܸ�λ��. ��֤scene���ҵ�entity
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
	L_ASSERT(other.m_type == EntityType::Player && m_type == EntityType::Player); //�Ƿ����ã�ֻ��player֮�����Ҫ����
	if (other.m_seePlayerNum >= (uint32_t)Entity::MAX_SEE_PLAYER)
	{//���������滻�����ˡ��� 
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
	m_scene->Freeze(true);//����scene�ı�״̬����Ϊscene �ı���ֻ���ı� entity ״̬
	m_isFreeze = true;
	for (Entity *p : m_playerObservers)
	{
		f(*p);//��֪��������ʲô�����Զ���ס�Ű�ȫ
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

//player ֮�� ���������ü�
//@entity ������Ұ���
//@vecAllPlayer entity��Ұ���������
void aoi::Scene::CalPlayerObserver(Entity& entity, const VecEntity &vecAllPlayer)
{
	//��ʼ��������Ұ��������б�
	PriList priList[(uint32_t)ViewPriolity::Max];    //�����б�0���ȼ����
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

	//��ʼ��entity �Ѽ���� ����	
	std::vector< Entity*> vecOldSeeOther;
	for (Entity* otherPlayer : vecAllPlayer)
	{
		if (otherPlayer->m_playerObservers.find(&entity) != otherPlayer->m_playerObservers.end())
		{
			vecOldSeeOther.push_back(otherPlayer);
		}
	}

	//�������ȼ�������player֮����Ұ
	std::vector< Entity*> vecNewSeeOther;//�¼���� �Ѽ���� ����	
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

	//vecOldSeeOther vecNewSeeOther ��� ִ�пɼ��䲻�ɼ�
	//���������ñ仯
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
	L_COND(!m_isFreeze, false);//����m_observers �ڼ��ֹ���ã� �û��跨��ʱ���ðɡ�
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
			{//player֮���ȼ�¼�������ٴ���
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
	if (vecAllPlayer.size() <= Entity::MAX_SEE_PLAYER)//����ʲ���������ִ��ȫ����
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
	L_COND(!m_isFreeze, false); //����m_observers �ڼ��ֹ���ã� �û��跨��ʱ���ðɡ�
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
	L_COND(!m_isFreeze, false);//����m_observers �ڼ��ֹ���ã� �û��跨��ʱ���ðɡ�
	L_COND(entity.GetScene(), false);
	m_isFreeze = true;
	if (GridIdxMgr::Ins().checkTwoPosIInNine(oldGridIdx, newGridIdx))
	{
	//	LDEBUG(&entity, "update in nine");
		uint8_t dir = GridIdxMgr::Ins().getScreenDirect(oldGridIdx, newGridIdx);
		{//����ɾ���������ڱ䲻�ɼ���Ұ
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
		{//��������Ұ������
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
							//otherEntity.TryAddObserver(*entity); //entity��Ұ�Ӻ�ü�����
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
			if (vecAllPlayer.size() <= Entity::MAX_SEE_PLAYER)//����ʲ���������ִ��ȫ����
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
		{//����ɾ���������ڱ䲻�ɼ���Ұ
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
					{//player֮���ȼ�¼�������ٴ���
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

			if (vecAllPlayer.size() <= Entity::MAX_SEE_PLAYER)//����ʲ���������ִ��ȫ����
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

