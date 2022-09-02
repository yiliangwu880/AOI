//代码结构使用演示，
#pragma once
#include "unit_test.h"
#include "aoi/aoi.h"
#include "aoi/GridIdxMgr.h"

namespace Demo
{
	class Player;
	class GameScene;
	struct SceneEntity : public aoi::Entity
	{
		Player &m_owner;
		SceneEntity(Player &player);
		std::set<Entity *> m_observers;

		virtual void OnAddObserver(Entity& other); //entity 看见我
		virtual void OnAddObserver(const std::vector<Entity*>& vecOther)
		{

		}
		virtual void OnDelView(Entity &other); //entity 看不见我
	};

	struct Player 
	{
		uint16_t m_x = 0;
		uint16_t m_y = 0;
		SceneEntity m_entity;
		uint32_t m_id = 0;
		GameScene *m_gameScene=nullptr;


	};



	struct GameScene 
	{
		aoi::Scene m_aoiScene;
	};

}