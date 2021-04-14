

#include "unit_test.h"
#include "aoi/aoi.h"
#include "aoi/GridIdxMgr.h"
#include "CheckAoi.h"

using namespace std;
using namespace aoi;


UNITTEST(tool_test)
{
	{
		uint16_t idx = Check::Ins().GetGridIdx(303, 44);
		VecGridIdx vecIdx = Check::Ins().Get9Grid(303, 44);
		VecGridIdx vecIdx2 = GridIdxMgr::Ins().Get9Grid(idx);

		for (uint16_t v : vecIdx)
		{
			bool isFind = false;
			for (uint16_t v2 : vecIdx2)
			{
				if (v == v2)
				{
					isFind = true;
				}
			}
			UNIT_ASSERT(isFind);
		}
		UNIT_ASSERT(vecIdx2.size() == vecIdx.size());
	}
	{
		VecGridIdx vecIdx = Check::Ins().Get9Grid(MAP_MAX_POS_X-1, MAP_MAX_POS_Y - 1);
		uint16_t t = Check::Ins().GetGridIdx(MAP_MAX_POS_X - 1, MAP_MAX_POS_Y - 1);
		VecGridIdx vecIdx2 = GridIdxMgr::Ins().Get9Grid(t);

		for (uint16_t v : vecIdx)
		{
			bool isFind = false;
			for (uint16_t v2 : vecIdx2)
			{
				if (v == v2)
				{
					isFind = true;
				}
			}
			//UNIT_INFO("%d %d", vecIdx.size(), vecIdx2.size());
			UNIT_ASSERT(isFind);
		}
		UNIT_ASSERT(vecIdx2.size() == vecIdx.size());
	}
	{
		VecGridIdx vecIdx = Check::Ins().Get9Grid(0, 0);
		VecGridIdx vecIdx2 = GridIdxMgr::Ins().Get9Grid(Check::Ins().GetGridIdx(0, 0));

		for (uint16_t v : vecIdx)
		{
			bool isFind = false;
			for (uint16_t v2 : vecIdx2)
			{
				if (v == v2)
				{
					isFind = true;
				}
			}
			UNIT_ASSERT(isFind);
		}
		UNIT_ASSERT(vecIdx2.size() == vecIdx.size());
	}

}

//测试 9格子可视 关系正确
UNITTEST(enter)
{
	GameScene *scene = new GameScene();
	PlayerMgr *mgr = new PlayerMgr();

	mgr->m_id2Player.insert(make_pair(1, Player(1)));
	mgr->m_id2Player.insert(make_pair(2, Player(2)));

	mgr->GetPlayer(1)->Enter(*scene, 371, 2014);
	mgr->GetPlayer(2)->Enter(*scene, 344, 2012);

	mgr->Check();
}
//测试 随意移动， 9格子可视 关系正确
UNITTEST(observer)
{
	GameScene *scene = new GameScene();
	PlayerMgr *mgr = new PlayerMgr();
	for (int i = 0; i < 200; i++)
	{
		mgr->m_id2Player.insert(make_pair(i, Player()));
	}

	for (auto &v : mgr->m_id2Player)
	{
		Player &player = v.second;
		player.Enter(*scene, 0, 0);
	}
	UNIT_ASSERT(199 == mgr->Check(0));
	mgr->GetPlayer(0)->Leave();
	UNIT_ASSERT(0 == mgr->Check(0));
	UNIT_ASSERT(198 == mgr->Check(1));

	for (auto &v : mgr->m_id2Player)
	{
		Player &player = v.second;
		player.Leave();
	}
	for (int i = 0; i < 200; i++)
	{
		UNIT_ASSERT(0 == mgr->Check(i));
	}


	delete mgr;
	delete scene;
}

//测试 删除对象1
UNITTEST(test_free)
{
	GameScene *scene = new GameScene();
	PlayerMgr *mgr = new PlayerMgr();
	for (int i = 0; i < 10; i++)
	{
		mgr->m_id2Player.insert(make_pair(i, Player()));
	}
	mgr->GetPlayer(0)->Enter(*scene, 0, 0);
	mgr->GetPlayer(1)->Enter(*scene, 11, 11);
	mgr->GetPlayer(2)->Enter(*scene, 0, 0);
	mgr->GetPlayer(3)->Enter(*scene, 303, 44);

	mgr->GetPlayer(2)->Leave();

	UNIT_ASSERT(3 == AoiTest::GetEntityNum(scene->m_aoiScene));

	UNIT_ASSERT(1 == mgr->Check(0));
	UNIT_ASSERT(1 == mgr->Check(1));
	UNIT_ASSERT(0 == mgr->Check(2));
	UNIT_ASSERT(0 == mgr->Check(3));

	mgr->GetPlayer(0)->Leave();
	{
		
		auto p = AoiTest::GetEntityScene(mgr->GetPlayer(1)->m_entity);
		UNIT_ASSERT(p);
	}
	delete scene;
	{
		auto p = AoiTest::GetEntityScene(mgr->GetPlayer(1)->m_entity);
		UNIT_ASSERT(!p);
	}

	for (int i = 0; i < 10; i++)
	{
		UNIT_ASSERT(0 == mgr->Check(i));
	}
	delete mgr;
}

//测试 删除对象2
UNITTEST(test_free2)
{
	GameScene *scene = new GameScene();
	PlayerMgr *mgr = new PlayerMgr();
	for (int i = 0; i < 10; i++)
	{
		mgr->m_id2Player.insert(make_pair(i, Player()));
	}
	mgr->GetPlayer(0)->Enter(*scene, 0, 0);
	mgr->GetPlayer(1)->Enter(*scene, 11, 11);
	mgr->GetPlayer(2)->Enter(*scene, 0, 0);
	//mgr->GetPlayer(3)->Enter(*scene, 8803, 4884);
	mgr->GetPlayer(3)->Enter(*scene, 803, 884);

	mgr->GetPlayer(2)->Leave();

	UNIT_ASSERT(1 == mgr->Check(0));
	UNIT_ASSERT(1 == mgr->Check(1));
	UNIT_ASSERT(0 == mgr->Check(2));
	UNIT_ASSERT(0 == mgr->Check(3));

	mgr->GetPlayer(0)->Leave();
	for (int i = 0; i < 10; i++)
	{
		UNIT_ASSERT(0 == mgr->Check(i));
	}

	UNIT_ASSERT(2 == AoiTest::GetEntityNum(scene->m_aoiScene));
	for (int i = 0; i < 10; i++)
	{
		mgr->GetPlayer(i)->Leave();
	}
	delete mgr;
	UNIT_ASSERT(0 == AoiTest::GetEntityNum(scene->m_aoiScene));

	delete scene;
}

//测试 最大坐标，最小坐标
UNITTEST(test_limit_xy)
{
	GameScene *scene = new GameScene();
	PlayerMgr *mgr = new PlayerMgr();
	for (int i = 0; i < 10; i++)
	{
		mgr->m_id2Player.insert(make_pair(i, Player()));
	}
	mgr->GetPlayer(0)->Enter(*scene, 0, 0);
	mgr->GetPlayer(1)->Enter(*scene, 0, 0);
	mgr->GetPlayer(2)->Enter(*scene, MAP_MAX_POS_X-1, MAP_MAX_POS_Y - 1);
	mgr->GetPlayer(3)->Enter(*scene, MAP_MAX_POS_X - 1, MAP_MAX_POS_Y - 1);

	UNIT_ASSERT(1 == mgr->Check(0));
	UNIT_ASSERT(1 == mgr->Check(1));
	UNIT_ASSERT(1 == mgr->Check(2));
	UNIT_ASSERT(1 == mgr->Check(3));

	mgr->GetPlayer(1)->Leave();
	mgr->GetPlayer(3)->Leave();

	UNIT_ASSERT(0 == mgr->Check(0));
	UNIT_ASSERT(0 == mgr->Check(2));

	for (int i = 0; i < 10; i++)
	{
		mgr->GetPlayer(i)->Leave();
	}
	delete mgr;
	delete scene;
}

//测试 一些坐标转换位置
UNITTEST(pos_cal)
{
	uint16_t x = 371, y = 2014;
	uint16_t centerGridX, centerGridY;
	uint16_t gridX, gridY;
	Check::Ins().GetGridXY(x, y, centerGridX, centerGridY);
	//UNIT_INFO("center %d, %d", centerGridX, centerGridY); //16, 143
	

	Check::Ins().GetGridXY(x - SCREEN_GRID_WIDTH, y - SCREEN_GRID_HEIGHT, gridX, gridY);
	UNIT_ASSERT(gridX = centerGridX - 1);
	UNIT_ASSERT(gridY = centerGridY - 1);
	Check::Ins().GetGridXY(x, y - SCREEN_GRID_HEIGHT, gridX, gridY);
	UNIT_ASSERT(gridX = centerGridX);
	UNIT_ASSERT(gridY = centerGridY - 1);
	Check::Ins().GetGridXY(x + SCREEN_GRID_WIDTH, y - SCREEN_GRID_HEIGHT, gridX, gridY);
	UNIT_ASSERT(gridX = centerGridX + 1);
	UNIT_ASSERT(gridY = centerGridY - 1);

	Check::Ins().GetGridXY(x - SCREEN_GRID_WIDTH, y , gridX, gridY);
	UNIT_ASSERT(gridX = centerGridX - 1);
	UNIT_ASSERT(gridY = centerGridY);
	Check::Ins().GetGridXY(x, y, gridX, gridY);
	UNIT_ASSERT(gridX = centerGridX);
	UNIT_ASSERT(gridY = centerGridY);
	Check::Ins().GetGridXY(x + SCREEN_GRID_WIDTH, y, gridX, gridY);
	UNIT_ASSERT(gridX = centerGridX + 1);
	UNIT_ASSERT(gridY = centerGridY);

	Check::Ins().GetGridXY(x - SCREEN_GRID_WIDTH, y + SCREEN_GRID_HEIGHT, gridX, gridY);
	UNIT_ASSERT(gridX = centerGridX - 1);
	UNIT_ASSERT(gridY = centerGridY + 1);
	Check::Ins().GetGridXY(x, y + SCREEN_GRID_HEIGHT, gridX, gridY);
	UNIT_ASSERT(gridX = centerGridX);
	UNIT_ASSERT(gridY = centerGridY + 1);
	Check::Ins().GetGridXY(x + SCREEN_GRID_WIDTH, y + SCREEN_GRID_HEIGHT, gridX, gridY);
	UNIT_ASSERT(gridX = centerGridX + 1);
	UNIT_ASSERT(gridY = centerGridY + 1);
}

//测试 进入，更新位置
UNITTEST(enter_update)
{
	GameScene *scene = new GameScene();
	GameScene *scene2 = new GameScene();
	PlayerMgr *mgr = new PlayerMgr();

	uint16_t x= 371, y= 2014;//gridxy = 16, 143
	uint16_t gridX, gridY;
	Check::Ins().GetGridXY(371, 2014, gridX, gridY);

	mgr->m_id2Player.insert(make_pair(1, Player(1)));
	mgr->m_id2Player.insert(make_pair(2, Player(2)));
	mgr->m_id2Player.insert(make_pair(3, Player(3)));

	mgr->GetPlayer(1)->Enter(*scene, x, y);
	mgr->GetPlayer(2)->Enter(*scene, x + SCREEN_GRID_WIDTH, y);
	mgr->GetPlayer(3)->Enter(*scene, x + SCREEN_GRID_WIDTH*3, y);
	UNIT_ASSERT(mgr->Check(1) == 1);
	UNIT_ASSERT(mgr->Check(2) == 1);
	UNIT_ASSERT(mgr->Check(3) == 0);
	mgr->Check();

	mgr->GetPlayer(1)->UpdateXY(x, y);
	mgr->GetPlayer(2)->UpdateXY(x, y);
	mgr->GetPlayer(3)->UpdateXY(x, y);
	UNIT_ASSERT(mgr->Check(1) == 2);
	UNIT_ASSERT(mgr->Check(2) == 2);
	UNIT_ASSERT(mgr->Check(3) == 2);
	mgr->Check();

	//站位 ：对角线排列 
	mgr->GetPlayer(1)->UpdateXY(x + SCREEN_GRID_WIDTH, y+ SCREEN_GRID_HEIGHT);
	mgr->GetPlayer(2)->UpdateXY(x , y );
	mgr->GetPlayer(3)->UpdateXY(x - SCREEN_GRID_WIDTH , y - SCREEN_GRID_HEIGHT);
	UNIT_ASSERT(mgr->Check(1) == 1);
	UNIT_ASSERT(mgr->Check(2) == 2);
	UNIT_ASSERT(mgr->Check(3) == 1);
	mgr->Check();

	mgr->GetPlayer(2)->UpdateXY(x - SCREEN_GRID_WIDTH*2, y - SCREEN_GRID_HEIGHT*2);
	UNIT_ASSERT(mgr->Check(1) == 0);
	UNIT_ASSERT(mgr->Check(2) == 1);
	UNIT_ASSERT(mgr->Check(3) == 1);
	mgr->Check();

	mgr->GetPlayer(2)->UpdateXY(x - SCREEN_GRID_WIDTH * 3, y - SCREEN_GRID_HEIGHT * 3);
	UNIT_ASSERT(mgr->Check(1) == 0);
	UNIT_ASSERT(mgr->Check(2) == 0);
	UNIT_ASSERT(mgr->Check(3) == 0);
	mgr->Check();

	//站位 ：对角线排列 
	mgr->GetPlayer(1)->UpdateXY(x + SCREEN_GRID_WIDTH, y + SCREEN_GRID_HEIGHT);
	mgr->GetPlayer(2)->UpdateXY(x, y);
	mgr->GetPlayer(3)->UpdateXY(x - SCREEN_GRID_WIDTH, y - SCREEN_GRID_HEIGHT);
	UNIT_ASSERT(mgr->Check(1) == 1);
	UNIT_ASSERT(mgr->Check(2) == 2);
	UNIT_ASSERT(mgr->Check(3) == 1);
	mgr->Check();
	{
		auto f = [&](Entity& other)
		{
			UNIT_ASSERT(&other == &(mgr->GetPlayer(2)->m_entity));
		};
		mgr->GetPlayer(1)->m_entity.ForEachObservers(f);
	}
	{
		auto f = [&](Entity& other)
		{
			UNIT_ASSERT(&other == &(mgr->GetPlayer(1)->m_entity) || &other == &(mgr->GetPlayer(3)->m_entity));
		};
		mgr->GetPlayer(2)->m_entity.ForEachObservers(f);
	}
	{
		auto f = [&](Entity& other)
		{
			UNIT_ASSERT(&other == &(mgr->GetPlayer(2)->m_entity));
		};
		mgr->GetPlayer(3)->m_entity.ForEachObservers(f);
	}

	//leave
	mgr->GetPlayer(1)->Enter(*scene2, 0, 0);
	UNIT_ASSERT(mgr->Check(1) == 0);
	UNIT_ASSERT(mgr->Check(2) == 1);
	UNIT_ASSERT(mgr->Check(3) == 1);
	mgr->Check();

	mgr->GetPlayer(1)->Enter(*scene, x + SCREEN_GRID_WIDTH, y + SCREEN_GRID_HEIGHT);
	UNIT_ASSERT(mgr->Check(1) == 1);
	UNIT_ASSERT(mgr->Check(2) == 2);
	UNIT_ASSERT(mgr->Check(3) == 1);
	mgr->Check();

	mgr->GetPlayer(1)->Enter(*scene, x + SCREEN_GRID_WIDTH, y + SCREEN_GRID_HEIGHT);
	mgr->GetPlayer(2)->Enter(*scene, x, y);
	mgr->GetPlayer(3)->Enter(*scene, x - SCREEN_GRID_WIDTH, y - SCREEN_GRID_HEIGHT);
	UNIT_ASSERT(mgr->Check(1) == 1);
	UNIT_ASSERT(mgr->Check(2) == 2);
	UNIT_ASSERT(mgr->Check(3) == 1);
	mgr->Check();

	mgr->GetPlayer(1)->Leave();	
	UNIT_ASSERT(mgr->Check(1) == 0);
	UNIT_ASSERT(mgr->Check(2) == 1);
	UNIT_ASSERT(mgr->Check(3) == 1);
	mgr->Check();

	mgr->GetPlayer(2)->Leave();
	UNIT_ASSERT(mgr->Check(1) == 0);
	UNIT_ASSERT(mgr->Check(2) == 0);
	UNIT_ASSERT(mgr->Check(3) == 0);
	mgr->Check();
}


//尝试重现
UNITTEST(test_update)
{
	GameScene *scene = new GameScene();
	PlayerMgr *mgr = new PlayerMgr();

	mgr->m_id2Player.insert(make_pair(1, Player(1)));
	mgr->m_id2Player.insert(make_pair(2, Player(2)));
	mgr->m_id2Player.insert(make_pair(3, Player(3)));

	mgr->GetPlayer(1)->Enter(*scene, 0, 0);
	mgr->GetPlayer(2)->Enter(*scene, 0, 0);
	mgr->GetPlayer(3)->Enter(*scene, 0, 0);

	mgr->Check();

	mgr->GetPlayer(1)->UpdateXY(277, 1881);//12 134
	mgr->GetPlayer(2)->UpdateXY(304, 1870);//13 133
	mgr->GetPlayer(3)->UpdateXY(304, 1870 - SCREEN_GRID_HEIGHT);
	mgr->Check();
}

//测试 随机进入场景， 随机移动
UNITTEST(rand_update_pos)
{
	GameScene *scene = new GameScene();
	GameScene *scene2 = new GameScene();
	PlayerMgr *mgr = new PlayerMgr();
	const int32_t MAX_NUM = 1000;
	for (int i = 0; i < MAX_NUM; i++)
	{
		mgr->m_id2Player.insert(make_pair(i, Player(i)));
	}
	for (int i = 0; i < MAX_NUM - 100; i++)
	{
		mgr->GetPlayer(i)->Enter(*scene, Check::Ins().RandX(), Check::Ins().RandY());
	}
	mgr->Check();
	for (int i = 0; i < 10000*10; i++)
	{
		int idx = Check::Ins().RandUint32(0, MAX_NUM - 100 - 1);
		int action = Check::Ins().RandUint32(0, 10);
		int sceneIdx = Check::Ins().RandUint32(0, 1);
		GameScene *pscene;
		if (0 == sceneIdx)
		{
			pscene = scene;
		}
		else
		{
			pscene = scene2;
		}
		if (0 == action)
		{
			mgr->GetPlayer(idx)->Enter(*pscene, Check::Ins().RandX(), Check::Ins().RandY());
		}
		else if (1 == action)
		{
			mgr->GetPlayer(idx)->Leave();
		}
		else
		{
			mgr->GetPlayer(idx)->UpdateXY(Check::Ins().RandX(), Check::Ins().RandY());
		}
		if (0 == i%1000)
		{
			mgr->Check();
		}
	}
	mgr->Check();

	for (int i = 0; i < MAX_NUM; i++)
	{
		mgr->GetPlayer(i)->Leave();
	}
	delete mgr;
	delete scene;
}

