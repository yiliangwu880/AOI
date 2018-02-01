/*
 @brief	屏索引
 */

#pragma once

#include "screen_type.h"
#include <array>

namespace
{
	//X,Y坐标调整
	const int WALK_ADJUST[9][2] = { { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, 0 } };
}

//@brief	屏幕索引关系管理器
class ScreenIdxMgr
{
private:
	ScreenIdxMgr();
	~ScreenIdxMgr(){};

public:	
	static ScreenIdxMgr &GetInstance();
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
	//@brief	检查两个屏是否是九屏关系
	/// @param	posIOne 第一个屏
	/// @param	posITwo 第二个屏
	/// @direct	针对posIOne的方向
	/// @return	如果posITwo是posIOne在direct方向上那一屏，返回true;否则返回false
	bool checkTwoPosIInNine(const ScreenIdx &posIOne, const ScreenIdx &posITwo, const uint8 &direct);
	//@brief	检查两个坐标编号是否有9屏关系
	/// @param	one 第一个编号
	/// @param	two 第二个编号
	/// @retturn	是否有9屏关系
	bool checkTwoPosIInNine(const ScreenIdx one, const ScreenIdx two);
	//@brief	根据屏编号得到九屏索引
	/// @param	posi 屏编号
	/// @return	九屏
	const VecSI &getNineScreen(const ScreenIdx &posi) const;
	//@brief 根据屏编号和方向得到前向屏
	/// @param posi 屏编号
	/// @param direct 方向
	/// @return 前向屏
	const VecSI &getDirectScreen(const ScreenIdx &posi, const int &direct) const;
	//@brief	根据屏编号和方向得到后向屏
	/// @param	posi 屏编号
	/// @param	direct 方向
	/// @return	后向屏
	const VecSI &getReverseDirectScreen(const ScreenIdx &posi, const int &direct) const;
	//@brief	获得屏之间的相对方向
	/// @param	posiorg 起始屏
	/// @param	posinew 另外一屏
	/// @return	方向
	DirType getScreenDirect(const ScreenIdx posiorg, const ScreenIdx posinew);
	//@brief	打印所有方向的前向屏
	void printAllDirectScreen(const ScreenIdx &posi);
private:
	//@brief	初始化屏关系
	/// @return	初始化是否成功
	bool initScreenIndex();
private:
	//@brief	九屏索引
	std::array<VecSI, MAP_SCREEN_MAX> m_ninescreen; //m_ninescreen[屏幕索引] == PosIVector，保存当前灯塔的<=9个屏幕
	//@brief	前向屏索引,二维数组[8][MAP_SCREEN_MAX]
	std::array<std::array<VecSI, MAP_SCREEN_MAX>, 8> m_direct_screen;//m_direct_screen[方向][屏幕索引]  == 屏幕列表（3个或者5个）
	//@brief	后向屏索引
	std::array<std::array<VecSI, MAP_SCREEN_MAX>, 8> m_reverse_direct_screen; //m_reverse_direct_screen[方向][屏幕索引]  == 屏幕列表（3个或者5个）
};
