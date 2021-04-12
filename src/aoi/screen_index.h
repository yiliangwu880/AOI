
#pragma once

#include <map>
#include <vector>
#include <unordered_map>
#include "utility.h"

namespace aoi
{

	//@brief	九屏关系容器
	typedef std::unordered_map<uint32_t, VecGridIdx> NineScreenMap;
	//@brief	九屏关系容器迭代器
	typedef NineScreenMap::iterator NineScreenMap_iter;
	//@brief	屏索引
	class ScreenIndexBase : public Singleton<ScreenIndexBase>
	{
		//@brief	九屏索引  m_ninescreen[uint16_t] 为uint16_t为中心的9个uint16_t
		 VecGridIdx m_ninescreen[MAP_SCREEN_MAX];
		//@brief	前向屏索引	m_direct_screen[DirType][uint16_t] 表示 uint16_t为中心，DirType方向的多个grid.最多5个
		 NineScreenMap m_direct_screen[8];
		//@brief	后向屏索引   m_reverse_direct_screen[DirType][uint16_t] 表示 uint16_t为中心，DirType方向的反方向的多个grid.最多5个
		 NineScreenMap m_reverse_direct_screen[8];

	public:
		ScreenIndexBase();

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
		const VecGridIdx &getNineScreen(const uint16_t &posi) const;
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
}