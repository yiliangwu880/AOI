/*
  9格子算法的 AOI(area of interest)。 
  管理 场景 实体 视野可见
使用方法：
   1）一切扩展接口都在Entity的公共函数 和虚函数中
   2）需要修改GridIdxMgr.h的最大地图长宽，grid长宽来匹配你的项目。
   3) 只需要include aoi.h
   4) 屏蔽 AOI_TEST 宏

特点：已经保证不引用野对象。
	保证方法：析构函数互相解引用，
			entity gridIdx变化保证调用 Scene::UpdateEntity
			ForEachObservers 遍历的时候，freeze scene ，不让entity gridIdx变化，离开，进入
	不提供阻挡，全场景实体遍历功能，这些不是AOI的任务。

术语：
	单位 point : 场景最小长度单位，也代表entity移动最小距离。地图坐标(x,y)得单位就是point
	单位 grid  : (gridIdx)场景视野划分得grid索引

使用例子：
	class ObserverEntity : public aoi::Entity
	class Player
	{
		ObserverEntity observer;
		...
	};
	class Scene
	{
		aoi::Scene aoiScene;
		...
	};
*/
#pragma once
#include <set>
#define AOI_TEST 1


namespace aoi
{
	class Scene;
	class Entity;

	class Entity
	{
		friend class Scene;


		Scene * m_scene = nullptr;
		std::set<Entity *> m_observers; //看见我的entity 集合
		uint16_t m_gridIdx = 0;			//当前所在格子索引
	public:
		~Entity();
		bool Enter(Scene &scene, uint16_t x, uint16_t y);
		bool Leave();
		void UpdatePos(uint16_t x, uint16_t y); //更新 x,y坐标。 单位为 point
		void ForEachObservers(std::function<void(Entity&)> f);//遍历 看见我的entity集合

	private:
		virtual void OnAddObserver(Entity &entity) = 0; //entity 看见我
		virtual void OnDelObserver(Entity &entity) = 0; //entity 看不见我

#if AOI_TEST
	public:
#else
	private:
#endif
		void SetScene(Scene *scene) { m_scene = scene; }
		void AddObserver(Entity &entity);//entity看见我
		void DelObserver(Entity &entity);//entity看不见我
		uint16_t GridIdx() const { return m_gridIdx; }
		void OnSceneDel();
		Scene *GetScene() const { return m_scene; }
	};

	class Scene
	{
		friend class Entity;

		using VecEntity = std::vector<Entity *>;
		//map 类型，不用设计地图大小，自动调整需要的内存。 如果数组类型，需要设置地图大小，效率更高
		std::map<uint16_t, VecEntity> m_idx2VecEntity;//gridIdx 2 entity. 表示一个grid的所有entity
		bool m_isFreeze = false; //true 表示禁止entity gridIdx变化。 防止野entity或者迭代过程修改容器

	public:
		~Scene(); //如果删除场景 还有entity,只做entity状态清理，不回调任何函数
		size_t GetEntityNum(); //for test use

	private:
		bool EntityEnter(Entity &entity);
		bool EntityLeave(Entity &entity);
		bool UpdateEntity(Entity &entity, uint16_t oldGridIdx, uint16_t newGridIdx); //entity gridIdx 发生变化
		
		bool Freeze() const { return m_isFreeze; }
		void Freeze(bool val) { m_isFreeze = val; }
	};

}