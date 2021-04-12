/*
  area of interest。 
  管理 场景 实体 视野可见

术语：
单位 point : 场景最小长度单位，也代表entity移动最小距离。地图坐标(x,y)得单位就是point
单位 grid  : (gridIdx)场景视野划分得grid索引, grid内的entity 能和周围8个grid可视
*/

#include "utility.h"
#include <set>

namespace aoi
{
	class Scene;


	//adpater ,适配将来不同的复杂实体
	class Entity
	{
		Scene * m_scene = nullptr;
		std::set<Entity *> m_observers; //看见我的entity 集合
		uint16_t m_gridIdx = 0;  //当前所在格子索引
	public:
		~Entity();
		bool GetScene() const { return m_scene; }
		void SetScene(Scene *scene) { m_scene = scene; }
		void AddObserver(Entity &entity);//entity看见我
		void DelObserver(Entity &entity);//entity看不见我
		void UpdatePos(uint16_t x, uint16_t y);
		uint16_t GridIdx() const { return m_gridIdx; }
	public:
		virtual Pos GetPos() = 0;
		virtual void OnAddObserver(Entity &entity) = 0;
		virtual void OnDelObserver(Entity &entity) = 0;
	};

	using VecEntity = std::vector<Entity *> ;

	//注意：需要用户保证entity Scene 对象 析构函数做了处理，保证互相不会引用野对象
	class Scene
	{
		//gridIdx 2 entity. 表示一个grid的所有entity
		std::map<uint32_t, VecEntity> m_posi2vecEntity;
		bool m_isForbidLeave = false; //true indicate forbid entity leave
	public:
		~Scene();
		bool EntityEnter(Entity &entity);
		bool EntityLeave(Entity &entity);
		bool UpdateEntity(Entity &entity, uint16_t oldGridIdx, uint16_t newGridIdx); //entity gridIdx 发生变化

	public:

	};

}