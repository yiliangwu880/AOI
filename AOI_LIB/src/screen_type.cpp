
#include "screen_type.h"
#include <math.h>

using namespace std;
Pix::Pix()
: x(0), y(0)
{
}

Pix::Pix(const Pix& p)
: x(p.x), y(p.y)
{
}

Pix::Pix(uint16 x_, uint16 y_)
: x(x_), y(y_)
{
}

Pix& Pix::operator=(const Pix &p)
{
	x = p.x;
	y = p.y;
	return *this;
}
//
//bool Pix::operator==(const Pix& p) const
//{
//	return p.x == x && p.y == y;
//}
//
////uint32 Pix::distance(const Pix& p) const
////{
////	return Algorithm::distance(*this, p);
////}
//
//bool Pix::operator!=(const Pix& p) const
//{
//	return !(operator==(p));
//}
//
//bool Pix::isInSamePos(const Pix& p) const
//{
//	return Pos(*this) == Pos(p);
//}
//
//Pos Pix::getPos() const
//{
//	return Pos(*this);
//}


Pos::Pos() : x(0), y(0){}

Pos::Pos(uint16 x, uint16 y) : x(x), y(y){}

Pos::Pos(const Pos &pos)
{
	x = pos.x;
	y = pos.y;
}

//Pos::Pos(const Pix& pix)
//{
//	x = pix.x / ONE_GRID_WIDTH;
//	y = pix.y / ONE_GRID_HEIGHT;
//}

bool Pos::valid() const
{
	return *this != INVALID_POS;
}

bool Pos::empty() const
{
	return x == 0 && y == 0;
}

bool Pos::checkInRange(const Pos& pos, const uint16& range) const
{
	return (abs(x - pos.x) <= range) && (abs(y - pos.y) <= range);
}

ScreenIdx Pos::GetPosI() const
{
	return valid() ? (MAP_SCREEN_X*(y / SCREEN_GRID_HEIGHT) + (x / SCREEN_GRID_WIDTH)) : INVALID_POSI;
}

Pos& Pos::operator= (const Pos &pos)
{
	x = pos.x;
	y = pos.y;
	return *this;
}

bool Pos::operator==(const Pos& pos) const
{
	return x == pos.x && y == pos.y;
}

bool Pos::operator!=(const Pos& pos) const
{
	return !operator==(pos);
}

uint32 Pos::hash() const
{
	return hash_value;
}
