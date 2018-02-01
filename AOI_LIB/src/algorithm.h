/**

 * @brief	游戏算法
 */

#pragma once
#include "config.h"
#include "utility/typedef.h"
#include <algorithm>
#include "game.h"

namespace Algorithm
{
	template<typename T>
	inline uint32 distance(const T& src, const T& des)
	{
		return static_cast<uint32>(sqrtf(powf((des.x - src.x), 2.0f) + powf((des.y - src.y), 2.0f)));
	}

	inline uint32 simpleDistance(const Pos& src, const Pos& des)
	{
		uint32 disx = abs(src.x - des.x);
		uint32 disy = abs(src.y - des.y);
		return std::min(disx, disy);
	}

	inline const int direct(const Pos& begin, const Pos& pos)
	{
		if(begin == pos)
		{
			return DirType_Random;
		}
		else if(begin.x == pos.x)
		{
			if(begin.y < pos.y)
			{
				return DirType_Down;
			}
			else
			{
				return DirType_Up;
			}
		}
		else if(begin.x < pos.x)
		{
			if(begin.y < pos.y)
			{
				return DirType_RightDown;
			}
			else if(begin.y == pos.y)
			{
				return DirType_Right;
			}
			else
			{
				return DirType_RightUp;
			}
		}
		else if(begin.x > pos.x)
		{
			if(begin.y < pos.y)
			{
				return DirType_LeftDown;
			}
			else if(begin.y == pos.y)
			{   
				return DirType_Left;
			}
			else
			{
				return DirType_LeftUp;
			}
		}
		return DirType_Wrong;
	}

	inline bool isInCircle(const Pos& center, uint32 r, const Pos& pos)
	{
		return r >= distance(center, pos);
	}

	//@brief	获取src到des直线路径上的点
	template<typename T>
	inline void getTSetInLine(const T& src, const T& des, OUT ZQGame::vector<T>& vec)
	{
		if(src == des)
		{
			vec.push_back(src);
		}
		else if(src.x == des.x)
		{
			T begin = src.y < des.y ? src : des;
			T end = src.y < des.y ? des : src;
			for(int32 y = begin.y; y <= end.y; ++y)
			{
				vec.push_back(T(src.x, y));
			}
		}
		else if(src.y == des.y)
		{
			T begin = src.x < des.x ? src : des;
			T end = src.x < des.x ? des : src;
			for(int32 x = begin.x; x <= end.x; ++x)
			{
				vec.push_back(T(x, begin.y));
			}
		}
		else
		{
			//直线方程：y=kx+y1-kx1;
			T begin = src.x < des.x ? src : des;
			T end = src.x < des.x ? des : src;
			double rate = ((double)(end.y - begin.y)) / ((double)(end.x - begin.x));	//可能为负数
			vec.push_back(begin);
			double last_y = begin.y + 0.5;
			for(int32 idx = 1; idx <= end.x - begin.x; ++idx)
			{
				double curr_y = last_y + rate;	//x变化1， y变化rate
				int32 curr_y_fix = (int32)floor(curr_y);
				int32 last_y_fix = (int32)floor(last_y);

				if(curr_y_fix == last_y_fix)
				{
					//y坐标未发生变化，则只有一格
					vec.push_back(T(begin.x + idx, (int32)curr_y));
					last_y = curr_y;
					continue;
				}

				//y坐标发生变化，考虑与y轴交点,交点两边的格子都是路径的经过点，都应放到vec中，交点之前的格子为(x+idx-1, y)
				//之后的为(x+idx, y)
				double inter_y = last_y + rate*0.5;			//线段中点y坐标

				int32 inter_y_fix = (int32)floor(inter_y);
				if(rate > 0)
				{
					//如果y坐标为整数，则第一段的y要-1
					if(inter_y == (double)(inter_y_fix))
					{
						--inter_y_fix;
					}

					for(int32 y = last_y_fix + 1; y <= inter_y_fix; ++y)
					{
						vec.push_back(T(begin.x + idx - 1, y));
					}
					for(int32 y = (int32)inter_y; y <= curr_y_fix; ++y)
					{
						vec.push_back(T(begin.x + idx, y));
					}
				}
				else
				{
					//如果y坐标为整数,则第二段y要-1
					if(inter_y == (double)(inter_y_fix))
					{
						--inter_y_fix;
					}

					for(int32 y = last_y_fix - 1; y >= inter_y; --y)
					{
						vec.push_back(T(begin.x + idx - 1, y));
					}
					for(int32 y = (int32)inter_y_fix; y >= curr_y_fix; --y)
					{
						vec.push_back(T(begin.x + idx, y));
					}
				}
				last_y = curr_y;
			}
		}

		//如果检查顺序并修正
		if(vec.size() >= 2 && vec[0] != src)
		{
			std::reverse(vec.begin(), vec.end());
		}
	}
#define getPosSetInLine getTSetInLine<Pos>


	//@brief	获取在园内的所有坐标, 直线距离
	inline void getPosSetInCircle(const Pos& center, const uint32 radius, OUT VecPos& vec)
	{
		uint16 minx = center.x > radius ? center.x - radius : 0;
		uint16 miny = center.y > radius ? center.y - radius : 0;
		uint16 maxx = center.x + radius;
		uint16 maxy = center.y + radius;
		for(uint16 x = minx; x <= maxx; ++x)
		{
			for(uint16 y = miny; y <= maxy; ++y)
			{
				Pos newone(x, y);
				if(Algorithm::distance(newone, center) <= radius)
				{
					vec.push_back(newone);
				}
			}
		}
	}

	//@brief	返回当天日期到天，如20120202表示2012年02月02日, cursec不填则取当前时间
	inline uint32 getCurDateByDay(uint32 cursec = 0)
	{
		time_t sec = (cursec == 0 ? time(0) : cursec);

		struct tm curr;
		bzero(&curr, sizeof(curr));
		Time::getLocalTime(curr, sec);
		uint32 ret = (curr.tm_year+1900) * 10000 + (curr.tm_mon+1)%100*100 + curr.tm_mday%100;
		return ret;
	}

	//@brief	判断输入的时间所在日期（到天）是否今天
	inline bool isSecondInCurDay(uint32 sec)
	{
		return sec && getCurDateByDay() == getCurDateByDay(sec);
	}

	//@brief	返回当天时间，如10203表示1点2分3秒，cursec不填则取当前时间
	inline uint32 getCurHMSByDay(uint32 cursec = 0)
	{
		time_t sec = (cursec == 0 ? time(0) : cursec);

		struct tm curr;
		bzero(&curr, sizeof(curr));
		localtime_r(&sec, &curr);
		uint32 ret = (curr.tm_hour) * 10000 + curr.tm_min%100*100 + curr.tm_sec%100;
		return ret;
	}

	//@brief	根据data描述，返回1970到data对应的当天时间，data为123456表示12点36分56秒
	inline uint32 getSecondPointByHour(uint32 data, uint32 cursec)
	{
		struct tm curr;
		bzero(&curr, sizeof(curr));
		Time::getLocalTime(curr, cursec);
		curr.tm_sec = data%100;
		curr.tm_min = data/100%100;
		curr.tm_hour = data/10000%100;
		return mktime(&curr);
	}

	//@brief	获得两点间的相对方向
	//@param	precise_arc 传出精确弧度(0, 2PI)
	inline uint32 getRelativeDir(int16 x1, int16 y1, int16 x2, int16 y2, double& precise_arc)
	{
		int16 delta_x = x2 - x1;
		int16 delta_y = y2 - y1;
		double tan = 0;
		double arc = 0;
		precise_arc = 0;

		if(delta_x == 0 && delta_y == 0)
			return RelativeDir_Unknown;
		if(delta_x >= 0)
		{
			if(delta_y == 0)
			{
				precise_arc = PIP2;
				return RelativeDir_East;
			}
			tan = (double)delta_x / (double)delta_y;
			arc = atan(tan);
			if(delta_y > 0)
			{
				precise_arc = arc;	//第1象限
				if(arc >= 0 && arc < PIP16)
					return RelativeDir_South;
				else if(arc >= PIP16 && arc <= 3.0 * PIP16)
					return RelativeDir_SouthEast;
				else
					return RelativeDir_East;
			}
			else
			{
				precise_arc = PI + arc;//arc为负数 第二象限
				if(arc >= -PIP4 && arc < -3.0 * PIP16)
					return RelativeDir_East;
				else if(arc >= -3.0 * PIP16 && arc <= -PIP16)
					return RelativeDir_NorthEast;
				else
					return RelativeDir_North;
			}
		}
		else
		{
			if(delta_y == 0)
			{
				precise_arc = 3 * PIP2;
				return RelativeDir_West;
			}
			tan = (double)delta_x / (double)delta_y;
			arc = atan(tan);
			if(delta_y > 0)
			{
				precise_arc = 2 * PI + arc;	//第4象限
				if(arc > -PIP16 && arc < 0)
					return RelativeDir_South;
				else if(arc >= -3.0 * PIP16 && arc <= -PIP16)
					return RelativeDir_SouthWest;
				else
					return RelativeDir_West;
			}
			else
			{
				precise_arc = PI + arc;	//第3象限
				if(arc > 0 && arc < PIP16)
					return RelativeDir_North;
				else if(arc >= PIP16 && arc <= 3.0 * PIP16)
					return RelativeDir_NortWest;
				else
					return RelativeDir_West;
			}
		}
	}

	//@brief	从[0,max)中随机出num个不同的数字
	inline void randNumSetByMax(uint32 max, uint32 num, ZQGame::vector<uint32>& out)
	{
		typedef ZQGame::vector<uint32> Uint32Vec;

		Uint32Vec tmp;
		tmp.resize(max);
		for(uint32 idx = 0; idx < max; ++idx)
		{
			tmp[idx] = idx;
		}

		num = std::min(num, max);

		out.clear();
		out.reserve(num);
		for(uint32 idx = 0; idx < num; ++idx)
		{
			uint32 rand_idx = ZQGame::randBetween(0, tmp.size() - 1);
			out.push_back(tmp[rand_idx]);
			tmp.erase(tmp.begin() + rand_idx);
		}
	}

	//@brief	从vector中随机选一个
	template<typename T>
	inline T randInVec(const ZQGame::vector<T>& in)
	{
		if(in.empty())
		{
			return T(0);
		}

		uint32 randidx = ZQGame::randBetween(0, in.size() - 1);
		if(randidx >= in.size())
		{
			return T(0);
		}

		return in[randidx];
	}

	//@brief	获取hms对应当天已经过去的秒数
	inline uint32 getCurDayHMSPast(uint32 hms)
	{
		CheckCondition(hms <= 240000, 0);
		return (hms%100 + hms/100%100*60 + hms/10000%100*60*60);
	}

	//@brief	获取俩个时间hhmmss的秒数差值
	inline uint32 getDiffSecondByHMS(uint32 total, uint32 sub)
	{
		CheckCondition(total > sub, 0);

		uint32 t1 = getCurDayHMSPast(total);
		uint32 t2 = getCurDayHMSPast(sub);
		return t1 - t2;
	}

	//@brief	获取从begin到下一个end的秒数，格式hms，end小于begin时返回到第二天的end的时间
	inline uint32 getDiffHMSNextDay(uint32 begin, uint32 end)
	{
		if(begin < end)
		{
			return getDiffSecondByHMS(end, begin);
		}
		else
		{
			//相等也当做第二天处理，第二天到end时间+今天剩余时间
			uint32 second_day = Algorithm::getCurDayHMSPast(end);
			uint32 cur_day_left = Algorithm::getDiffSecondByHMS(240000, begin);
			return cur_day_left + second_day;
		}
	}

	//@brief	获取下一分钟时间，HHMMSS，如当前为125903，则返回130000
	inline uint32 getNextHMS(uint32 cur)
	{
		uint32 hh = cur/10000;
		uint32 mm = cur/100%100;
		if(cur >= 235959 || (hh == 23 && mm == 59))
		{
			return 0;
		}
		else if(mm >= 59)
		{
			return (hh+1)*10000;
		}
		else
		{
			return hh*10000 + (mm+1)*100;
		}
	}

	//@brief    根据字符串返回对应1970到当时的时间，格式2013-05-01 2:3:4                
	inline uint32 getTimeTByString(const char* in)
	{
		struct tm tm_s;
		strptime(in, "%Y-%m-%d %H:%M:%S", &tm_s);
		return (uint32)(mktime(&tm_s));
	}

	//@brief    非线程安全，根据time_t返回对应时间的字符串                              
	inline const char* getTimeStringByTimeT(time_t tm)
	{
		static char buf[128];
		bzero(buf, sizeof(buf));
		struct tm* tmp = localtime(&tm);
		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmp);
		return buf;
	}

	//@brief	打印容器
	template<typename T>
	inline void printSimpleColl(const T& coll, const char* tip)
	{
		std::ostringstream oss;
		oss << "\n-----------" << tip << " begin-----------\n";
		uint32 i = 1;
		for(typename T::const_iterator it = coll.begin(), end = coll.end(); it != end; ++it, ++i)
		{
			if(i > 1)
			{
				oss << ", ";
			}
			oss << "(" << *it << ")";
			if(i % 10 == 0)
			{
				oss << "\n";
			}
		}
		oss << "\n-----------" << tip << " end  -----------";
		INFO("%s", oss.str().c_str());
	}

	inline bool isPosCompleteChnWordStart(const char* word, size_t pos)
	{
		if(word == NULL)
		{
			return false;
		}

		if(pos == 0)
		{
			return true;
		}

		//从pos往前走，汉字占用的字节为偶数
		uint32 num = 0;
		--pos;
		while(((const uint8*)word)[pos] >= 0x81)
		{
			++num;

			if(pos == 0)
			{
				break;
			}
			--pos;
		}

		return num%2 == 0;
	}

	template<typename TSrc, typename TDel>
		inline void removeByInvalid(TSrc& src, TDel& del)
		{
			for(typename TDel::const_iterator it = del.begin(), end = del.end(); it != end; ++it)
			{
				src.erase(*it);
			}
		}
	//把秒转换成时，分，秒字符串
	inline void fmtReadableTime(std::ostringstream& oss, uint32 sec)
	{
		uint16 h = sec / 3600;
		uint16 m = (sec % 3600) / 60;
		uint16 s = sec % 60;
		if(h)
		{
			oss << h << "时";
		}
		if(m)
		{
			oss << m << "分";
		}
		if(s)
		{
			oss << s << "秒";
		}
	}

	inline void pushVString(std::string& out, const char* fmt, ...)
	{
		char buf[1024];
		bzero(buf, sizeof(buf));
		FMTString(buf, sizeof(buf), fmt);
		out += buf;
	}

	template<typename T>
	inline int32 randByWeight(const ZQGame::vector<T>& in)
	{
		if(in.empty())
		{
			return -1;
		}

		uint32 sum = 0;
		ZQGame::vector<uint32> sum_list;
		for(uint32 i = 0; i < in.size(); ++i)
		{
			sum += in[i].weight;
			sum_list.push_back(sum);
		}
		if(sum == 0)
		{
			return 0;
		}

		uint32 rand_weight = ZQGame::randBetween(1, sum);
		for(uint32 i = 0; i < in.size(); ++i)
		{
			if(rand_weight <= sum_list[i])
			{
				return i;
			}
		}

		return 0;
	}
};
