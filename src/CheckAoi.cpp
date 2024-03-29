
#include "CheckAoi.h"
#include "aoi/log_def.h"

using namespace std;
using namespace aoi;

uint16_t Check::GetGridIdx(uint16_t x, uint16_t y)
{
	if (x >= MAP_MAX_POS_X || y >= MAP_MAX_POS_Y)
	{
		UNIT_ERROR("x,y is too big %d %d", x, y);
		return -1;
	}
	return (MAP_SCREEN_X * (y / SCREEN_GRID_HEIGHT) + (x / SCREEN_GRID_WIDTH));
}

uint16_t Check::GetGridIdxByGridXY(uint16_t gridX, uint16_t gridY)
{
	return gridY * MAP_SCREEN_X + gridX;
}

void Check::GetGridXY(uint16_t idx, uint16_t &gridX, uint16_t &gridY)
{
	if (idx >= MAP_SCREEN_MAX)
	{
		UNIT_ERROR("idx overload %d", idx);
		return;
	}
	gridY = idx / MAP_SCREEN_X;
	gridX = idx % MAP_SCREEN_X;
}

void Check::GetGridXY(uint16_t x, uint16_t y, uint16_t &gridX, uint16_t &gridY)
{
	uint16_t idx = GetGridIdx(x, y);
	GetGridXY(idx, gridX, gridY);
}

aoi::VecGridIdx Check::Get9Grid(uint16_t idx)
{
	VecGridIdx vec;
	if (idx >= MAP_SCREEN_MAX)
	{
		UNIT_ERROR("idx overload %d", idx);
		return vec;
	}
	int gridX, gridY;
	gridY = idx / MAP_SCREEN_X;
	gridX = idx % MAP_SCREEN_X;
	
	//UNIT_INFO("xIdx,yIdx=%d %d --%d (%d %d)", xIdx, yIdx, idx, MAP_SCREEN_X, MAP_SCREEN_Y);
	//9格
	for (int x = -1; x < 2; x++)
	{
		for (int y = -1; y < 2; y++)
		{
			if (gridX + x < 0)
			{
				continue;
			}
			if (gridX + x >= MAP_SCREEN_X)
			{
				continue;
			}if (gridY + y < 0)
			{
				continue;
			}
			if (gridY + y >= MAP_SCREEN_Y)
			{
				continue;
			}
			vec.push_back((gridY + y)*MAP_SCREEN_X +gridX + x);
		}
	}
	return vec;
}

VecGridIdx Check::Get9Grid(uint16_t x, uint16_t y)
{
	return Get9Grid(GetGridIdx(x, y));
}

void Check::Compare(uint16_t oldGridIdx, uint16_t newGridIdx, GridCompareResult &ret)
{
	VecGridIdx vecNew = Get9Grid(newGridIdx);
	VecGridIdx vecOld = Get9Grid(oldGridIdx);

	for (uint16_t newIdx : vecNew)
	{
		for (uint16_t oldIdx : vecOld)
		{
			if (newIdx == oldIdx)
			{
				ret.same.push_back(oldIdx);
			}
			else
			{
				ret.enterNew.push_back(newIdx);
				ret.leaveOld.push_back(oldIdx);
			}
		}
	}
}

uint16_t Check::RandPos()
{
	static bool isInit = false;
	if (!isInit)
	{
		srand((unsigned int)time(0));
		isInit = true;
	}

	uint32_t r =  (::rand() << 17) | (::rand() << 2) | (::rand());
	return (uint16_t)r;
}
//返回 闭区间[min,max]内随机一个数
uint32_t Check::RandUint32(uint32_t min, uint32_t max)
{
	if (min == max)
		return min;
	else if (min > max) {
		return max + rand32() % (min - max + 1);
	}
	else {
		return min + rand32() % (max - min + 1);
	}
}

uint32_t Check::rand32()
{
	//确保 RAND_MAX == 0x7fff 才有效
	return (::rand() << 17) | (::rand() << 2) | (::rand());
}

SceneEntity::SceneEntity(Player &player)
	:m_owner(player)
{

}

void SceneEntity::OnAddObserver(Entity &other)
{
	m_playerObservers.insert(&other);
}

void SceneEntity::OnDelObserver(Entity &other)
{
	m_playerObservers.erase(&other);
}

Player::Player()
	:m_entity(*this)
{

}

Player::Player(uint32_t id)
	: m_entity(*this),
	m_id(id)
{

}

Player::Player(const Player &other)
	: m_entity(*this),
	m_id(other.m_id)
{

}

bool Player::Enter(GameScene &scene, uint16_t x, uint16_t y)
{
	m_x = x;
	m_y = y;
	//UNIT_INFO("%d - %d %d", m_id, m_x, m_y);
	m_gameScene = &scene;
	return m_entity.Enter(scene.m_aoiScene, x, y);
}

bool Player::Leave()
{
	m_gameScene = nullptr;
	return m_entity.Leave();
}

void Player::UpdateXY(uint16_t x, uint16_t y)
{
	m_x = x;
	m_y = y;
	m_entity.UpdatePos(m_x, m_y);
}

uint32_t PlayerMgr::Check(uint32_t id)
{
	auto it = m_id2Player.find(id);
	if (it == m_id2Player.end())
	{
		UNIT_ERROR("find player id fail %d", id)
		return 0;
	}
	Player &player = it->second;
	return Check(player);
}

void PlayerMgr::Check()
{
	for (auto &v : m_id2Player)
	{
		Player &player = v.second;
		Check(player);
	}
}

uint32_t PlayerMgr::Check(Player &player)
{
	set< aoi::Entity *> g_set; //全局查找9grid entity集合
	{
		if (player.m_gameScene)
		{
			VecGridIdx vecIdx = Check::Ins().Get9Grid(player.m_x, player.m_y);
			for (uint16_t idx : vecIdx)
			{
				vector<aoi::Entity *> vecEntity = GetGridEntity(*player.m_gameScene, idx);
				g_set.insert(vecEntity.begin(), vecEntity.end());

			}
		}
	}

	set<aoi::Entity *> aoi_set;//player.m_entity 视野 目标数量
	{
		auto f = [&aoi_set](Entity& other)
		{
			aoi_set.insert(&other);
		};
		player.m_entity.ForEachObservers(f);
	}

	{//aoi_set 必须为 g_set子集
		for (auto i : aoi_set)
		{
			UNIT_ASSERT(g_set.find(i) != g_set.end());
		}
	}

	if (Entity::MAX_SEE_PLAYER > 100) //最大可视玩家数量过大，忽略裁剪。当全部都看到测试
	{
		uint32_t cnt = 0;
		for (aoi::Entity *g_entity : g_set)
		{
			if (g_entity == &player.m_entity)
			{
				continue;
			}
	#if 0
			{//for test
				if (aoi_set.find(g_entity) == aoi_set.end())//when error
				{
					UNIT_INFO("player id=%d, gridIdx=%d x,y=%d, %d", player.m_id, player.m_entity.GridIdx(), player.m_x, player.m_y);
					uint16_t x, y;
					Check::Ins().GetGridXY(player.m_entity.GridIdx(), x, y);
					UNIT_INFO("gridXY = %d %d", x, y);
					//UNIT_INFO("aoi_set size, g_set size=%d %d", aoi_set.size(), g_set.size());

					SceneEntity *sceneEntity = dynamic_cast<SceneEntity *>(g_entity);
					//UNIT_INFO("g_set entity scene = %p", sceneEntity->GetScene());
					UNIT_INFO("g_set entity id=%d, gridIdx=%d x,y=%d, %d", sceneEntity->m_owner.m_id, g_entity->GridIdx(), sceneEntity->m_owner.m_x, sceneEntity->m_owner.m_y);
					Check::Ins().GetGridXY(g_entity->GridIdx(), x, y);
					UNIT_INFO("gridXY = %d %d", x, y);
					//UNIT_INFO("g_set player  = %p", &sceneEntity->m_owner);

					for (aoi::Entity *aoi_entity : aoi_set)
					{
						SceneEntity *sceneEntity = dynamic_cast<SceneEntity *>(aoi_entity);
						UNIT_INFO("aoi entity id=%d, gridIdx=%d x,y=%d, %d", sceneEntity->m_owner.m_id, aoi_entity->GridIdx(), sceneEntity->m_owner.m_x, sceneEntity->m_owner.m_y);
						uint16_t x, y;
						Check::Ins().GetGridXY(aoi_entity->GridIdx(), x, y);
						UNIT_INFO("gridXY = %d %d", x, y);
					}
				}	
			}
	#endif
			//LDEBUG("aoi_set.szie()", aoi_set.size());
			UNIT_ASSERT(aoi_set.find(g_entity) != aoi_set.end()); 
			cnt++;
		}
		//LDEBUG("aoi_set.size(), 9 grid cnt", aoi_set.size(), cnt);
		UNIT_ASSERT(aoi_set.size() == cnt);
	}
	return aoi_set.size();
}

Player * PlayerMgr::GetPlayer(uint32_t id)
{
	auto it = m_id2Player.find(id);
	if (it == m_id2Player.end())
	{
		return nullptr;
	}
	return &(it->second);
}

std::vector<aoi::Entity *> PlayerMgr::GetGridEntity(GameScene &gameScene, uint16_t idx)
{
	std::vector<aoi::Entity *> vec;
	for (auto &v : m_id2Player)
	{
		Player &player = v.second;
		Scene *pScene = AoiTest::GetEntityScene(player.m_entity);
		if (pScene != nullptr && pScene == &(gameScene.m_aoiScene))
		{
			uint16_t i = Check::Ins().GetGridIdx(player.m_x, player.m_y);
			if (i == idx)
			{
				//{//for test
				//	Player *rp = &(player.m_entity.m_owner);
				//	UNIT_INFO("m_owner id scene %d %p", player.m_entity.m_owner.m_id, player.m_entity.m_owner.m_gameScene);
				//	UNIT_INFO("rp id scene %d %p", rp->m_id, rp->m_gameScene);
				//	UNIT_INFO("push_back player %d %p, m_entity=%p, reback player %p", player.m_id, &player, &(player.m_entity), rp);
				//}
				vec.push_back(&(player.m_entity));
			}
		}
	}
	return vec;
}
