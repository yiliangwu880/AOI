/**

 */

#include "screen_index.h"
#include <stdlib.h>
using namespace std;

ScreenIdxMgr & ScreenIdxMgr::GetInstance()
{
	static ScreenIdxMgr obj;
	return obj;
}

ScreenIdxMgr::ScreenIdxMgr()
{
	initScreenIndex();
}

bool ScreenIdxMgr::initScreenIndex()
{
	//预先建立地图九屏索引
	for(uint32 idx=0; idx < MAP_SCREEN_MAX; idx++)
	{
		//屏幕索引分解出X,Y索引
		int nScreenX = idx % MAP_SCREEN_X;
		int nScreenY = idx / MAP_SCREEN_X;
		//计算周围九屏
		{
			VecSI pv;
			for(int i = 0; i < 9; i++) {
				int x = nScreenX + WALK_ADJUST[i][0];
				int y = nScreenY + WALK_ADJUST[i][1];
				if(x >= 0 && y >= 0 && x < (int)MAP_SCREEN_X && y < (int)MAP_SCREEN_Y) {
					pv.push_back(y * MAP_SCREEN_X + x); 
				} 
			}
			m_ninescreen[idx] = pv;
		}
		//计算正向变化五屏或者三屏
		for(int dir = 0; dir < 8; dir++)
		{
			int start, end;
			VecSI pv;

			if(1 == dir % 2) {
				//斜方向
				start = 6;
				end = 10;
			}
			else {
				//正方向
				start = 7;
				end = 9;
			}
			for(int i = start; i <= end; i++) {
				int x = nScreenX + WALK_ADJUST[(i + dir) % 8][0];
				int y = nScreenY + WALK_ADJUST[(i + dir) % 8][1];
				if(x >= 0 && y >= 0 && x < (int)MAP_SCREEN_X && y < (int)MAP_SCREEN_Y) {
					pv.push_back(y * MAP_SCREEN_X + x);
				}
			}
			m_direct_screen[dir][idx]=pv;
		}
		//计算反向变化五屏或者三屏
		for(int dir = 0; dir < 8; dir++)
		{
			int start, end;
			VecSI pv;

			if(1 == dir % 2) {
				//斜方向
				start = 2;
				end = 6;
			}
			else {
				//正方向
				start = 3;
				end = 5;
			}
			for(int i = start; i <= end; i++) {
				int x = nScreenX + WALK_ADJUST[(i + dir) % 8][0];
				int y = nScreenY + WALK_ADJUST[(i + dir) % 8][1];
				if(x >= 0 && y >= 0 && x < (int)MAP_SCREEN_X && y < (int)MAP_SCREEN_Y) {
					pv.push_back(y * MAP_SCREEN_X + x);
				}
			}
			m_reverse_direct_screen[dir][idx]=pv;
		}
	}
	return true;
} 




bool ScreenIdxMgr::checkTwoPosIInNine(const ScreenIdx &posIOne, const ScreenIdx &posITwo, const uint8 &direct)
{
	return m_ninescreen[posIOne][direct] == posITwo;
}

bool ScreenIdxMgr::checkTwoPosIInNine(const ScreenIdx one, const ScreenIdx two)
{   
	int oneX, oneY , twoX ,twoY;
	oneX = one % MAP_SCREEN_X;   
	oneY = one / MAP_SCREEN_X;   
	twoX = two % MAP_SCREEN_X;
	twoY = two / MAP_SCREEN_X;
	if (::labs(oneX - twoX) <= 1 && ::labs(oneY - twoY) <= 1)
	{   
		return true;
	}   
	return false;   
} 

ScreenIdxMgr::DirType ScreenIdxMgr::getScreenDirect(const ScreenIdx posiorg, const ScreenIdx posinew)
{
	static ScreenIdxMgr::DirType dir[3][3] =
	{
		{DirType_LeftUp, DirType_Left, DirType_LeftDown},
		{DirType_Up, DirType_Wrong, DirType_Down},
		{DirType_RightUp, DirType_Right, DirType_RightDown},
	};
	uint32 diff_X = (posinew % MAP_SCREEN_X) - (posiorg % MAP_SCREEN_X) + 1;
	uint32 diff_Y = (posinew / MAP_SCREEN_X) - (posiorg / MAP_SCREEN_X) + 1;
	if (labs(diff_X) > 2 || labs(diff_Y) > 2)
	{
		return DirType_Wrong;
	}
	return dir[diff_X][diff_Y];
}

const VecSI &ScreenIdxMgr::getNineScreen(const ScreenIdx &posi) const
{
	return m_ninescreen[posi];
}

const VecSI &ScreenIdxMgr::getDirectScreen(const ScreenIdx &posi, const int &dir) const
{
	return m_direct_screen[dir][posi];
}

const VecSI &ScreenIdxMgr::getReverseDirectScreen(const ScreenIdx &posi, const int &dir) const
{
	return m_reverse_direct_screen[dir][posi];
}

void ScreenIdxMgr::printAllDirectScreen(const ScreenIdx &posi)
{
	LOG_DEBUG("前向屏 [%u]", posi);
	for (uint32 i = 0; i<m_direct_screen.size(); ++i)
	{
		LOG_DEBUG("方向 [%u]", i);
		if ((uint32)posi < m_direct_screen[0].size())
		{
			const VecSI &v = m_direct_screen[i][(uint32)posi];
			for (const auto &idx : v)
			{
				LOG_DEBUG("屏编号 [%u]", idx);
			}
		}
	}
}
