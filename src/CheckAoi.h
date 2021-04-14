#pragma once
#include "unit_test.h"
#include "aoi/aoi.h"
#include "aoi/GridIdxMgr.h"

using VecGridIdx = aoi::VecGridIdx;
struct GridCompareResult
{
	VecGridIdx leaveOld;//离开旧的grid
	VecGridIdx enterNew;//进入新的grid
	VecGridIdx same; //相同
};
//全局的计算方法，校验aoi
class Check
{
public:
	static Check& Ins()
	{
		static Check obj;
		return obj;
	}


	uint16_t GetGridIdx(uint16_t x, uint16_t y);

	uint16_t GetGridIdxByGridXY(uint16_t gridX, uint16_t gridY);

	VecGridIdx Get9Grid(uint16_t idx);
	VecGridIdx Get9Grid(uint16_t x, uint16_t y);

	void GetGridXY(uint16_t idx, uint16_t &gridX, uint16_t &gridY);
	void GetGridXY(uint16_t x, uint16_t y, uint16_t &gridX, uint16_t &gridY);

	//2idx为中心的grid比较，获取差异信息
	void Compare(uint16_t oldGridIdx, uint16_t newGridIdx, GridCompareResult &ret);
	
	uint16_t RandPos();
	uint16_t RandX() { return RandPos() % aoi::MAP_MAX_POS_X; };
	uint16_t RandY() { return RandPos() % aoi::MAP_MAX_POS_Y; };
	//返回 闭区间[min,max]内随机一个数
	static uint32_t RandUint32(uint32_t min, uint32_t max);
	static uint32_t rand32();
};

struct Player;
struct GameScene;
struct SceneEntity : public aoi::Entity
{
	Player &m_owner;
	SceneEntity(Player &player);
	std::set<Entity *> m_observers;

	virtual void OnAddObserver(Entity &other); //entity 看见我
	virtual void OnDelObserver(Entity &other); //entity 看不见我
};

struct Player 
{
	uint16_t m_x = 0;
	uint16_t m_y = 0;
	SceneEntity m_entity;
	uint32_t m_id = 0;
	GameScene *m_gameScene=nullptr;

	Player();
	Player(uint32_t id);
	Player(const Player &other);
	bool Enter(GameScene &scene, uint16_t x, uint16_t y);
	bool Leave();
	void UpdateXY(uint16_t x, uint16_t y);

};

struct PlayerMgr
{
	std::map<uint32_t, Player> m_id2Player;

	Player *GetPlayer(uint32_t id);

	//和全局搜索aoi比较,校验正确性. 并返回周围entity数量
	uint32_t Check(uint32_t id);
	uint32_t Check(Player &player);
	void Check();

	//全局算法获取格子所有 entity
	std::vector<aoi::Entity *> GetGridEntity(GameScene &gameScene, uint16_t idx);

};

struct GameScene 
{
	aoi::Scene m_aoiScene;
};

