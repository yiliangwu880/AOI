#pragma once

#include <map>
#include <vector>
#include <unordered_map>

namespace aoi
{
	static const uint16_t SCREEN_GRID_WIDTH = 22;		///< grid宽,单位 point
	static const uint16_t SCREEN_GRID_HEIGHT = 14;		///< grid高,单位 point
	//2048, 2048//最大地图长度,单位 point。 x,y 0开始，索引最大值为 （MAP_MAX_POS_X-1）
	static const uint16_t MAP_MAX_POS_X = 2200;
	static const uint16_t MAP_MAX_POS_Y = 2048;

	//索引从0开始
	//MAP_SCREEN_MAX 表示 grid 索引的最大值 + 1 
	//比如 MAP_SCREEN_X MAP_SCREEN_Y MAP_SCREEN_MAX == 10 10 100
	static const uint16_t MAP_SCREEN_X = (MAP_MAX_POS_X + SCREEN_GRID_WIDTH - 1) / SCREEN_GRID_WIDTH;  
	static const uint16_t MAP_SCREEN_Y = (MAP_MAX_POS_Y + SCREEN_GRID_HEIGHT - 1) / SCREEN_GRID_HEIGHT;
	static const uint32_t MAP_SCREEN_MAX = MAP_SCREEN_X * MAP_SCREEN_Y  ;//map 的最大grid数  

	typedef std::vector<uint16_t> VecGridIdx;
	enum DirType
	{
		DirType_Up = 0,	/// 向上
		DirType_RightUp = 1,	/// 右上
		DirType_Right = 2,	/// 向右
		DirType_RightDown = 3,	/// 右下
		DirType_Down = 4,	/// 向下
		DirType_LeftDown = 5,	/// 左下
		DirType_Left = 6,	/// 向左
		DirType_LeftUp = 7,	/// 左上
		DirType_Wrong = 8,	/// 错误方向
		DirType_Random = 8,	/// 随机方向
	};

	//@brief	九屏关系容器
	typedef std::unordered_map<uint32_t, VecGridIdx> NineScreenMap;
	//@brief	九屏关系容器迭代器
	typedef NineScreenMap::iterator NineScreenMap_iter;
	//@brief	屏索引
	class GridIdxMgr 
	{
		//@brief	九屏索引  m_ninescreen[uint16_t] 为uint16_t为中心的9个uint16_t
		 VecGridIdx m_ninescreen[MAP_SCREEN_MAX];
		//@brief	前向屏索引	m_direct_screen[DirType][uint16_t] 表示 uint16_t为中心，DirType方向的多个grid.最多5个
		 NineScreenMap m_direct_screen[8];
		//@brief	后向屏索引   m_reverse_direct_screen[DirType][uint16_t] 表示 uint16_t为中心，DirType方向的反方向的多个grid.最多5个
		 NineScreenMap m_reverse_direct_screen[8];

	public:
		static GridIdxMgr& Ins()
		{
			static GridIdxMgr obj;
			return obj;
		}

	private:
		GridIdxMgr();

	public:
		//@brief	初始化屏关系
		/// @return	初始化是否成功
		 bool initScreenIndex();
		//@brief	检查两个屏是否是九屏关系
		/// @param	posIOne 第一个屏
		/// @param	posITwo 第二个屏
		/// @direct	针对posIOne的方向
		/// @return	如果posITwo是posIOne在direct方向上那一屏，返回true;否则返回false
		 bool checkTwoPosIInNine(const uint16_t &posIOne, const uint16_t &posITwo, const uint8_t &direct);
		//@brief	检查两个坐标编号是否有9屏关系
		/// @param	one 第一个编号
		/// @param	two 第二个编号
		/// @retturn	是否有9屏关系
		 bool checkTwoPosIInNine(const uint16_t one, const uint16_t two);
		//@brief	根据屏编号得到九屏索引
		/// @param	posi 屏编号
		/// @return	九屏
		const VecGridIdx &Get9Grid(const uint16_t &posi) const;
		//@brief 根据屏编号和方向得到前向屏
		/// @param posi 屏编号
		/// @param direct 方向
		/// @return 前向屏
		const VecGridIdx &getDirectScreen(const uint16_t &posi, const int &direct) ;
		//@brief	根据屏编号和方向得到后向屏
		/// @param	posi 屏编号
		/// @param	direct 方向
		/// @return	后向屏
		const VecGridIdx &getReverseDirectScreen(const uint16_t &posi, const int &direct) ;
		//@brief	获得屏之间的相对方向
		/// @param	posiorg 起始屏
		/// @param	posinew 另外一屏
		/// @return	方向
		 uint8_t getScreenDirect(const uint16_t posiorg, const uint16_t posinew);
		//@brief	打印所有方向的前向屏
		 void printAllDirectScreen(const uint16_t &posi);
		//@brief	打印所有方向的前向屏
		 void printAllReverseDirectScreen(const uint16_t &posi);

	};


	template <typename T, typename Alloc>
	inline void SimpleRemoveFromVec(std::vector<T, Alloc >& vec, T val)
	{
		for (typename std::vector<T, Alloc >::iterator iter = vec.begin(); iter != vec.end(); ++iter)
		{
			if (*iter == val)
			{
				*iter = vec.back();
				vec.erase(vec.end() - 1);
				return;
			}
		}
	}

	template <typename T, typename Alloc>
	inline void SimpleRemoveFromVec(std::vector<T, Alloc >& vec, size_t index)
	{
		vec[index] = vec[vec.size() - 1];
		vec.erase(vec.end() - 1);
	}

	template <typename T, typename Alloc>
	inline void SimpleRemoveFromVec(std::vector<T, Alloc >& vec, typename std::vector<T, Alloc >::iterator it)
	{
		*it = vec.back();
		vec.erase(vec.end() - 1);
	}

	template <typename T>
	inline void SimpleRemoveFromVec(std::vector<T>& vec, T val)
	{
		for (typename std::vector<T>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
		{
			if (*iter == val)
			{
				*iter = vec.back();
				vec.erase(vec.end() - 1);
				return;
			}
		}
	}

	template <typename T>
	inline void SimpleRemoveFromVec(std::vector<T>& vec, size_t index)
	{
		vec[index] = vec[vec.size() - 1];
		vec.erase(vec.end() - 1);
	}

	template <typename T>
	inline void SimpleRemoveFromVec(std::vector<T>& vec, typename std::vector<T>::iterator it)
	{
		*it = vec.back();
		vec.erase(vec.end() - 1);
	}
}