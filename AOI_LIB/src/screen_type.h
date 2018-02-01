/*
brief:
*/
#pragma once
#include "config.h"
#include "utility/typedef.h"
#include <vector>

namespace
{
	//最大地图的size. 像素为单位 (最小移动计量单位，不一定是客户端图片像素)
	const uint32 MAX_MAP_X = 2048;
	const uint32 MAX_MAP_Y = 2048;

	const uint32 SCREEN_GRID_WIDTH = 22;		///< 单屏宽
	const uint32 SCREEN_GRID_HEIGHT = 14;		///< 单屏高 

	//最大屏幕数量
	const uint16 MAP_SCREEN_X = (MAX_MAP_X + SCREEN_GRID_WIDTH - 1) / SCREEN_GRID_WIDTH;
	const uint16 MAP_SCREEN_Y = (MAX_MAP_Y + SCREEN_GRID_HEIGHT - 1) / SCREEN_GRID_HEIGHT;
	const uint32 MAP_SCREEN_MAX = MAP_SCREEN_X * MAP_SCREEN_Y + 1;
}

typedef uint16 ScreenIdx;//位置索引

struct Pix;
//@brief	格子坐标
struct Pos
{
	union
	{
		struct
		{
			uint16 x;
			uint16 y;
		};
		uint32 hash_value;
		uint16 value[2];
	};

	Pos();
	Pos(uint16 x, uint16 y);
	Pos(const Pos& pos);
	//Pos(const Pix& pos);
	ScreenIdx GetPosI() const;
	bool valid() const;
	bool empty() const;
	bool checkInRange(const Pos& pos, const uint16& range) const;
	uint32 hash() const;
	Pos& operator=(const Pos& pos);
	bool operator==(const Pos& pos) const;
	bool operator!=(const Pos& pos) const;
};

//@brief	像素位置
struct Pix
{
	union
	{
		struct
		{
			uint16 x;
			uint16 y;
		};
		uint32 hash_value;
	};

	Pix();
	Pix(const Pix& p);
	Pix(uint16 x_, uint16 y_);
	Pix& operator=(const Pix& p);
	bool operator==(const Pix& p) const;
	bool operator!=(const Pix& p) const;
	uint32 distance(const Pix& p) const;
	bool isInSamePos(const Pix& p) const;
	Pos getPos() const;
};


typedef std::vector<ScreenIdx> VecSI;
typedef std::vector<Pos> VecPos;
namespace{
const ScreenIdx INVALID_POSI = (const ScreenIdx)-1;
const VecSI ZERO_POSI;
const Pos INVALID_POS(-1, -1);
}