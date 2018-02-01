/************************************************************************/
/*
Author:         Yiliangwu
*/
/************************************************************************/

#pragma once

#include "../utility/typedef.h"
#include <vector>

typedef  unsigned short U16;
/* ---------------------------------------------------------------------------
    RANDOM NUMBER GENERATOR
    This library provide the functions to generate random number sequence,
    all the number in this sequence will not repeat within a round
	
	
   ------------------------------------------------------------------------ */
class PrimeRandom
{
public:	
	//参数U16 maxNum表示 能随机的总数。从0开始。 比如参数=2，只能随机出0,1
	explicit PrimeRandom(U16 maxNum);
	U16 GetRandom();
private:
	void Init(U16 maxNum);
	U16 oRandom( U16 range );
	void RandomMode();
	void GetRightNext();
	void GetLeftNext();
	U16 GetNext(bool isLeft);



private:
	U16 m_modeStartIndex;
	U16 m_step;
	U16 m_mode;
	U16 m_right;
	U16 m_left;
	U16 m_start;
	U16 m_base;

	static const U16 PrimeNumbersTable[];
	static const size_t REMAIN_RANG = 6;
	static const U16 MAX_PRIME;
	static const U16 PRIME_LEN;
};


/************************************************************************/
/* 
规则：权重随机，比如[0]=10, [1]=90.随机1的几率是90%
*/
/************************************************************************/
class RandomWeight
{
public:
	RandomWeight(const std::vector<uint32> &vecWeight);
	uint32 rand();	
	//para vecWeight 为随机权重数组
	//return 索引，从0开始
	static uint32 rand(const std::vector<uint32> &vecWeight);

private:
	std::vector<uint32> m_sumWeight;
	uint32 m_sum;
};

namespace wyl
{
	void InitSeed();
	//闭区间内随机一个数, 有缺点， 不能随机过大
	uint32 randBetween(uint32 min, uint32 max);
	int GetRandomNumber(int iMin, int iMax);//闭区间内随机一个数

    uint32 rand32();
    // 生成[low, high]之间的无重复的cnt个随机数
    //如果区间跨度大，效率会低。建议用PrimeRandom类
    bool GetIntervalNoRepeatedRandNum(U16 low, U16 high, U16 cnt, U16 *num);
}//namespace wyl