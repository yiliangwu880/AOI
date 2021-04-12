/*
  9格子算法的 AOI(area of interest)。 
  管理 场景 实体 视野可见

特点：已经保证不引用野对象。
	保证方法：析构函数互相解引用，
			entity gridIdx变化保证调用 Scene::UpdateEntity
			ForEachObservers 遍历的时候，freeze scene ，不让entity gridIdx变化，离开，进入
		

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

#include <set>

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
		bool Leave(Scene &scene);
		void UpdatePos(uint16_t x, uint16_t y); //更新 x,y坐标。 单位为 point
		void ForEachObservers(std::function<void(Entity&)> f);//遍历 看见我的entity集合

	private:
		virtual void OnAddObserver(Entity &entity) = 0; //entity 看见我
		virtual void OnDelObserver(Entity &entity) = 0; //entity 看不见我

	private:
		Scene *GetScene() const { return m_scene; }
		void SetScene(Scene *scene) { m_scene = scene; }
		void AddObserver(Entity &entity);//entity看见我
		void DelObserver(Entity &entity);//entity看不见我
		uint16_t GridIdx() const { return m_gridIdx; }
		void OnSceneDel();
	};

	class Scene
	{
		friend class Entity;

		using VecEntity = std::vector<Entity *>;
		std::map<uint32_t, VecEntity> m_idx2VecEntity;//gridIdx 2 entity. 表示一个grid的所有entity
		bool m_isFreeze = false; //true 表示禁止entity gridIdx变化。 防止野entity或者迭代过程修改容器

	public:
		~Scene(); //如果删除场景 还有entity,只做entity状态清理，不回调任何函数

	private:
		bool EntityEnter(Entity &entity);
		bool EntityLeave(Entity &entity);
		bool UpdateEntity(Entity &entity, uint16_t oldGridIdx, uint16_t newGridIdx); //entity gridIdx 发生变化
		
		bool Freeze() const { return m_isFreeze; }
		void Freeze(bool val) { m_isFreeze = val; }
	};

}