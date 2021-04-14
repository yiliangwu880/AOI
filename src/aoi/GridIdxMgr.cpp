#include "GridIdxMgr.h"
#include "stdlib.h"
#include "log_def.h"

using namespace aoi;


GridIdxMgr::GridIdxMgr()
	:m_ninescreen()
{	
	initScreenIndex();
}

bool GridIdxMgr::initScreenIndex()
{
	const int adjust[9][2] = { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, 0} };
	//预先建立地图九屏索引
	for(uint32_t j=0; j < MAP_SCREEN_MAX ; j ++)
	{
		int gridX = j % MAP_SCREEN_X;
		int gridY = j / MAP_SCREEN_X;
		//计算周围九屏
		{
			VecGridIdx pv;
			for(int i = 0; i < 9; i++) {
				int x = gridX + adjust[i][0];
				int y = gridY + adjust[i][1];
				if(x >= 0 && y >= 0 && x < (int)MAP_SCREEN_X && y < (int)MAP_SCREEN_Y) {
					pv.push_back(y * MAP_SCREEN_X + x); 
				} 
			}
			m_ninescreen[j] = pv;
		}
		//计算正向变化五屏或者三屏
		for(int dir = 0; dir < 8; dir++)
		{
			int start, end;
			VecGridIdx pv;

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
				int x = gridX + adjust[(i + dir) % 8][0];
				int y = gridY + adjust[(i + dir) % 8][1];
				if(x >= 0 && y >= 0 && x < (int)MAP_SCREEN_X && y < (int)MAP_SCREEN_Y) {
					pv.push_back(y * MAP_SCREEN_X + x);
				}
			}
			m_direct_screen[dir].insert(make_pair(j,pv));
		}
		//计算反向变化五屏或者三屏
		for(int dir = 0; dir < 8; dir++)
		{
			int start, end;
			VecGridIdx pv;

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
				int x = gridX + adjust[(i + dir) % 8][0];
				int y = gridY + adjust[(i + dir) % 8][1];
				if(x >= 0 && y >= 0 && x < (int)MAP_SCREEN_X && y < (int)MAP_SCREEN_Y) {
					pv.push_back(y * MAP_SCREEN_X + x);
				}
			}
			m_reverse_direct_screen[dir].insert(make_pair(j,pv));
		}
	}
	return true;
} 

bool GridIdxMgr::checkTwoPosIInNine(const uint16_t &posIOne, const uint16_t &posITwo, const uint8_t &direct)
{
	return (m_ninescreen[posIOne][direct] == posITwo);
}

bool GridIdxMgr::checkTwoPosIInNine(const uint16_t one, const uint16_t two)
{   
	int oneX, oneY , twoX ,twoY;
	oneX = one % MAP_SCREEN_X;   
	oneY = one / MAP_SCREEN_X;   
	twoX = two % MAP_SCREEN_X;
	twoY = two / MAP_SCREEN_X;
	if(std::abs(oneX - twoX) <=1 && std::abs(oneY - twoY) <=1)
	{   
		return true;
	}   
	return false;   
} 

uint8_t GridIdxMgr::getScreenDirect(const uint16_t posiorg, const uint16_t posinew)
{
	static uint8_t dir[3][3] = 
	{
		{DirType_LeftUp, DirType_Left, DirType_LeftDown},
		{DirType_Up, DirType_Wrong, DirType_Down},
		{DirType_RightUp, DirType_Right, DirType_RightDown},
	};
	int32_t diff_X = (posinew % MAP_SCREEN_X) - (posiorg % MAP_SCREEN_X) + 1;
	int32_t diff_Y = (posinew / MAP_SCREEN_X) - (posiorg / MAP_SCREEN_X) + 1;
	if(abs(diff_X) > 2 || abs(diff_Y) > 2)
	{
		return DirType_Wrong;
	}
	return dir[diff_X][diff_Y];
}

const VecGridIdx &GridIdxMgr::Get9Grid(const uint16_t &posi) const
{
	L_ASSERT((uint32_t)posi < MAP_SCREEN_MAX);

	if(posi < MAP_SCREEN_MAX)
	{
		return m_ninescreen[posi];
	}

	L_ASSERT(false && "九屏索引异常");
	return *(VecGridIdx*)nullptr;
}

const VecGridIdx &GridIdxMgr::getDirectScreen(const uint16_t &posi, const int &dir) 
{
	L_ASSERT((uint32_t)posi < MAP_SCREEN_MAX);
	L_ASSERT(dir < DirType_Wrong);

	auto iter = m_direct_screen[dir].find((uint32_t)posi);
	if(iter != m_direct_screen[dir].end())
	{
		return iter->second;
	}

	L_ASSERT(false && "前向屏索引异常");

	return m_direct_screen[dir][(uint32_t)-1];
}

const VecGridIdx &GridIdxMgr::getReverseDirectScreen(const uint16_t &posi, const int &dir) 
{
	L_ASSERT((uint32_t)posi < MAP_SCREEN_MAX);
	L_ASSERT(dir < DirType_Wrong);

	NineScreenMap_iter iter = m_reverse_direct_screen[dir].find((uint32_t)posi);
	if(iter != m_reverse_direct_screen[dir].end())
	{
		return iter->second;
	}

	L_DEBUG("反向屏索引异常%d", dir);

	return m_reverse_direct_screen[dir][(uint32_t)-1];
}

void GridIdxMgr::printAllDirectScreen(const uint16_t &posi)
{
	L_DEBUG("前向屏 [%u]", posi);
	for(uint32_t i = 0; i<DirType_Wrong; ++ i)
	{
		L_DEBUG("方向 [%u]", i);
		NineScreenMap_iter iter = m_direct_screen[i].find((uint32_t)posi);
		if(iter != m_direct_screen[i].end())
		{
			for(VecGridIdx::iterator posiIter = iter->second.begin(); posiIter != iter->second.end(); ++posiIter)
			{
				L_DEBUG("屏编号 [%u]", *posiIter);
			}
		}
	}
}

void aoi::GridIdxMgr::printAllReverseDirectScreen(const uint16_t &posi)
{
	L_DEBUG("前向屏 [%u]", posi);
	for (uint32_t i = 0; i < DirType_Wrong; ++i)
	{
		L_DEBUG("方向 [%u]", i);
		NineScreenMap_iter iter = m_reverse_direct_screen[i].find((uint32_t)posi);
		if (iter != m_reverse_direct_screen[i].end())
		{
			for (VecGridIdx::iterator posiIter = iter->second.begin(); posiIter != iter->second.end(); ++posiIter)
			{
				L_DEBUG("屏编号 [%u]", *posiIter);
			}
		}
	}
}
