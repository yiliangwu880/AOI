/**

 * @brief	��Ϸ�㷨
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

	//@brief	��ȡsrc��desֱ��·���ϵĵ�
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
			//ֱ�߷��̣�y=kx+y1-kx1;
			T begin = src.x < des.x ? src : des;
			T end = src.x < des.x ? des : src;
			double rate = ((double)(end.y - begin.y)) / ((double)(end.x - begin.x));	//����Ϊ����
			vec.push_back(begin);
			double last_y = begin.y + 0.5;
			for(int32 idx = 1; idx <= end.x - begin.x; ++idx)
			{
				double curr_y = last_y + rate;	//x�仯1�� y�仯rate
				int32 curr_y_fix = (int32)floor(curr_y);
				int32 last_y_fix = (int32)floor(last_y);

				if(curr_y_fix == last_y_fix)
				{
					//y����δ�����仯����ֻ��һ��
					vec.push_back(T(begin.x + idx, (int32)curr_y));
					last_y = curr_y;
					continue;
				}

				//y���귢���仯��������y�ύ��,�������ߵĸ��Ӷ���·���ľ����㣬��Ӧ�ŵ�vec�У�����֮ǰ�ĸ���Ϊ(x+idx-1, y)
				//֮���Ϊ(x+idx, y)
				double inter_y = last_y + rate*0.5;			//�߶��е�y����

				int32 inter_y_fix = (int32)floor(inter_y);
				if(rate > 0)
				{
					//���y����Ϊ���������һ�ε�yҪ-1
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
					//���y����Ϊ����,��ڶ���yҪ-1
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

		//������˳������
		if(vec.size() >= 2 && vec[0] != src)
		{
			std::reverse(vec.begin(), vec.end());
		}
	}
#define getPosSetInLine getTSetInLine<Pos>


	//@brief	��ȡ��԰�ڵ���������, ֱ�߾���
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

	//@brief	���ص������ڵ��죬��20120202��ʾ2012��02��02��, cursec������ȡ��ǰʱ��
	inline uint32 getCurDateByDay(uint32 cursec = 0)
	{
		time_t sec = (cursec == 0 ? time(0) : cursec);

		struct tm curr;
		bzero(&curr, sizeof(curr));
		Time::getLocalTime(curr, sec);
		uint32 ret = (curr.tm_year+1900) * 10000 + (curr.tm_mon+1)%100*100 + curr.tm_mday%100;
		return ret;
	}

	//@brief	�ж������ʱ���������ڣ����죩�Ƿ����
	inline bool isSecondInCurDay(uint32 sec)
	{
		return sec && getCurDateByDay() == getCurDateByDay(sec);
	}

	//@brief	���ص���ʱ�䣬��10203��ʾ1��2��3�룬cursec������ȡ��ǰʱ��
	inline uint32 getCurHMSByDay(uint32 cursec = 0)
	{
		time_t sec = (cursec == 0 ? time(0) : cursec);

		struct tm curr;
		bzero(&curr, sizeof(curr));
		localtime_r(&sec, &curr);
		uint32 ret = (curr.tm_hour) * 10000 + curr.tm_min%100*100 + curr.tm_sec%100;
		return ret;
	}

	//@brief	����data����������1970��data��Ӧ�ĵ���ʱ�䣬dataΪ123456��ʾ12��36��56��
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

	//@brief	�����������Է���
	//@param	precise_arc ������ȷ����(0, 2PI)
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
				precise_arc = arc;	//��1����
				if(arc >= 0 && arc < PIP16)
					return RelativeDir_South;
				else if(arc >= PIP16 && arc <= 3.0 * PIP16)
					return RelativeDir_SouthEast;
				else
					return RelativeDir_East;
			}
			else
			{
				precise_arc = PI + arc;//arcΪ���� �ڶ�����
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
				precise_arc = 2 * PI + arc;	//��4����
				if(arc > -PIP16 && arc < 0)
					return RelativeDir_South;
				else if(arc >= -3.0 * PIP16 && arc <= -PIP16)
					return RelativeDir_SouthWest;
				else
					return RelativeDir_West;
			}
			else
			{
				precise_arc = PI + arc;	//��3����
				if(arc > 0 && arc < PIP16)
					return RelativeDir_North;
				else if(arc >= PIP16 && arc <= 3.0 * PIP16)
					return RelativeDir_NortWest;
				else
					return RelativeDir_West;
			}
		}
	}

	//@brief	��[0,max)�������num����ͬ������
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

	//@brief	��vector�����ѡһ��
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

	//@brief	��ȡhms��Ӧ�����Ѿ���ȥ������
	inline uint32 getCurDayHMSPast(uint32 hms)
	{
		CheckCondition(hms <= 240000, 0);
		return (hms%100 + hms/100%100*60 + hms/10000%100*60*60);
	}

	//@brief	��ȡ����ʱ��hhmmss��������ֵ
	inline uint32 getDiffSecondByHMS(uint32 total, uint32 sub)
	{
		CheckCondition(total > sub, 0);

		uint32 t1 = getCurDayHMSPast(total);
		uint32 t2 = getCurDayHMSPast(sub);
		return t1 - t2;
	}

	//@brief	��ȡ��begin����һ��end����������ʽhms��endС��beginʱ���ص��ڶ����end��ʱ��
	inline uint32 getDiffHMSNextDay(uint32 begin, uint32 end)
	{
		if(begin < end)
		{
			return getDiffSecondByHMS(end, begin);
		}
		else
		{
			//���Ҳ�����ڶ��촦���ڶ��쵽endʱ��+����ʣ��ʱ��
			uint32 second_day = Algorithm::getCurDayHMSPast(end);
			uint32 cur_day_left = Algorithm::getDiffSecondByHMS(240000, begin);
			return cur_day_left + second_day;
		}
	}

	//@brief	��ȡ��һ����ʱ�䣬HHMMSS���統ǰΪ125903���򷵻�130000
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

	//@brief    �����ַ������ض�Ӧ1970����ʱ��ʱ�䣬��ʽ2013-05-01 2:3:4                
	inline uint32 getTimeTByString(const char* in)
	{
		struct tm tm_s;
		strptime(in, "%Y-%m-%d %H:%M:%S", &tm_s);
		return (uint32)(mktime(&tm_s));
	}

	//@brief    ���̰߳�ȫ������time_t���ض�Ӧʱ����ַ���                              
	inline const char* getTimeStringByTimeT(time_t tm)
	{
		static char buf[128];
		bzero(buf, sizeof(buf));
		struct tm* tmp = localtime(&tm);
		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmp);
		return buf;
	}

	//@brief	��ӡ����
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

		//��pos��ǰ�ߣ�����ռ�õ��ֽ�Ϊż��
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
	//����ת����ʱ���֣����ַ���
	inline void fmtReadableTime(std::ostringstream& oss, uint32 sec)
	{
		uint16 h = sec / 3600;
		uint16 m = (sec % 3600) / 60;
		uint16 s = sec % 60;
		if(h)
		{
			oss << h << "ʱ";
		}
		if(m)
		{
			oss << m << "��";
		}
		if(s)
		{
			oss << s << "��";
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
