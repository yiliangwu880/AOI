#pragma once
#include <string>
#include "log_def.h"


namespace aoi
{

	//static const uint16_t MAP_SCREEN_X;
	//static const uint16_t MAP_SCREEN_Y;
	//static const uint32_t MAP_SCREEN_MAX;

	class Noncopyable
	{
	protected:
		Noncopyable() {};
		~Noncopyable() {};

	private:
		Noncopyable(const Noncopyable&) = delete;
		Noncopyable & operator= (const Noncopyable &) = delete;
		Noncopyable(Noncopyable&&) = delete; //移动构造函数
	};


	template <typename T>
	class Singleton : private Noncopyable
	{
	protected:
		Singleton() {}
		virtual ~Singleton() {}

	public:
		static T& Ins()
		{
			if (!m_obj)
				m_obj = new T;

			return *m_obj;
		}

		static void Free()
		{
			if (m_obj)
				delete m_obj;

			m_obj = nullptr;
		}

	private:
		static T* m_obj;

	};

	template<class T>
	T* Singleton<T>::m_obj = nullptr;  //初始化



	union Pos
	{
		struct
		{
			uint16_t x;
			uint16_t y;
		};
		uint32_t hash_value;

		Pos();
		Pos(uint16_t x, uint16_t y);
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
	static const uint16_t SCREEN_GRID_WIDTH = 22;	///< grid宽,单位 point
	static const uint16_t SCREEN_GRID_HEIGHT = 14;		///< grid高,单位 point
	//static const uint16_t ONE_GRID_WIDTH = 32;		///< 一个格子的像素长度
	//static const uint16_t ONE_GRID_HEIGHT = 32;		///< 一个格子的像素高度
	//2048, 2048//最大地图,单位 point
	static const uint16_t MAP_MAX_POS_X = 2200;
	static const uint16_t MAP_MAX_POS_Y = 2048;

	//map的最大长度，单位grid
	static const uint16_t MAP_SCREEN_X = (MAP_MAX_POS_X + SCREEN_GRID_WIDTH - 1) / SCREEN_GRID_WIDTH;
	static const uint16_t MAP_SCREEN_Y = (MAP_MAX_POS_Y + SCREEN_GRID_HEIGHT - 1) / SCREEN_GRID_HEIGHT;
	static const uint32_t MAP_SCREEN_MAX = MAP_SCREEN_X * MAP_SCREEN_Y + 1;//map 的最大grid数

	typedef std::vector<uint16_t> VecGridIdx;

}