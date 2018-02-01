/* ---------------------------------------------------------------------------
    RANDOM NUMBER GENERATOR
    This library provide the functions to generate random number sequence,
    all the number in this sequence will not repeat within a round
	
	Author:         Yiliangwu
	Version:        1.0
   ------------------------------------------------------------------------ */

#include "../utility/misc.h"
#include "random.h"

//using namespace std;
//using namespace Yiliang;


//extern PrimeNumbers_TAB;	
const U16 PrimeRandom::PrimeNumbersTable[] = {
	53		,  
	113        ,  
	263        ,  
	353        ,  
	463        ,  
	593        ,  
	733        ,  
	883        ,  
	1013       ,  
	2053       ,  
	3163       ,
	4177       ,
	5323       ,
	6353       ,
	7537       ,
	8353       ,
	9623       ,
	10631      ,
	11593      ,
	12613      ,
	13313      ,
	14423      ,
	15733      ,
	16433      ,
	17383      ,
	18433      ,
	19423      ,
	20773      ,
	21433      ,
	22303      ,
	23203      ,
	24413      ,
	25343      ,
	26053      ,
	27283      ,
	28513      ,
	29663      ,
	30713      ,
	31513      ,
	32303      ,
	33223      ,
	34313      ,
	35323      ,
	36293      ,
	37223      ,
	38303      ,
	39233      ,
	40483      ,
	41813      ,
	42533      ,
	43403      ,
	44383      ,
	45413      ,
	46523      ,
	47513      ,
	48473      ,
	49333      ,
	50333      ,
	51343      ,
	52313      ,
	53323      ,
	54323      ,
	55333      ,
	56333      ,
	57373      ,
	58313      ,
	59333      ,
	60343      ,
	61333      ,
	62323      ,
	63313      ,
	64303      ,
	65413      ,
	65437      ,
	65447      ,	//max
	65449      ,
	65479      ,
	65497      ,
	65519      ,
	65521
};
const U16 PrimeRandom::PRIME_LEN = static_cast<U16>(wyl::ArrayLen(PrimeNumbersTable));
const U16 PrimeRandom::MAX_PRIME = PrimeNumbersTable[PRIME_LEN-REMAIN_RANG];

void PrimeRandom::RandomMode()
{
	m_mode = PrimeNumbersTable[m_modeStartIndex + oRandom(REMAIN_RANG)];
}

void PrimeRandom::GetRightNext()
{
	//calculate next random number with formula:
	// An = (A(n-1) + Step) mode Z;
	do
	{
		m_right = (m_right + m_step)% m_mode;
	}while(m_right > m_base);
}

void PrimeRandom::GetLeftNext()
{
	//calculate next random number with formula:
	// A(n-1) = (An+(Z-Step))mod Z;
	do
	{
		m_left = (m_left+(m_mode-m_step)) % m_mode;
	}while(m_left > m_base);
}

U16 PrimeRandom::GetNext(bool isLeft)
{
	int iRet;
	
	iRet = (isLeft) ? m_left:m_right;
	//if all number have been fetch out, reset the group;
	if(m_right == m_left)
	{
		m_right = m_start;
		m_left = m_start;
		// If rand base is small, don't change the step and prime number
		if(m_base > 13)
		{
			m_step = oRandom(m_base>>2)+m_base/13+1; //oRandom(iRandBase[iGroup]/4)+iRandBase[iGroup]/13+1;
			RandomMode();
		}
		GetRightNext();
	}
	else
	{
		if(isLeft) 
		{
			GetLeftNext();
		} 
		else
		{
			GetRightNext();
		}
	}
	return (U16)iRet;
}

void PrimeRandom::Init(U16 maxNum)
{
	if (0 == maxNum)
	{
		maxNum = 1;
	}
	if(maxNum > MAX_PRIME) 
	{
		maxNum = 1;  //Can't create the random more than 65497; 弄个1，让他明显失败，容易查出问题
	}
	m_base = maxNum-1;

	size_t i;
	for( i = 0; i < PRIME_LEN; i++)
	{
		if(PrimeNumbersTable[i] > m_base) break;
	}
	m_modeStartIndex = (U16)i;

	if(maxNum <= 1 )
	{
		m_mode = maxNum;
		return;
	}
	
	if(m_base > 13)
	{
		m_step = oRandom(m_base>>2)+m_base/13+1;	//oRandom(iRandBase[iGroup]/4)+iRandBase[iGroup]/13+1;
	}
	else
	{
		m_step = oRandom(maxNum>>1)+1;			    //oRandom(iBase/2)+1;
	}

	m_start = oRandom(maxNum);

	m_left = m_start;
	m_right = m_start;
	RandomMode();
	GetRightNext();
}

U16 PrimeRandom::GetRandom()
{
	if(m_base == 0)	
		return 0;
 	return GetNext((oRandom(100)&0x1));
}

U16 PrimeRandom::oRandom( U16 range )
{
	return rand()%range;
}

PrimeRandom::PrimeRandom( U16 maxNum )
{
	Init(maxNum);
}


RandomWeight::RandomWeight(const std::vector<uint32> &vecWeight )
{
	m_sum = 0;
	for (std::vector<uint32>::const_iterator iter=vecWeight.begin();
		iter!=vecWeight.end(); ++iter)
	{
		m_sum += *iter;
		m_sumWeight.push_back(m_sum);
	}
}

uint32 RandomWeight::rand()
{
	if (0 == m_sum)
	{
		return 0;
	}
	uint32 randWeight = ::rand()%m_sum + 1;
	uint32 i=0;
	for (std::vector<uint32>::const_iterator iter=m_sumWeight.begin();
		iter!=m_sumWeight.end(); ++iter, ++i)
	{	
		if(randWeight <= *iter)
		{
			return i;
		}
	}
	return 0;
}

uint32 RandomWeight::rand( const std::vector<uint32> &vecWeight )
{
	uint32 sum = 0;
	for (std::vector<uint32>::const_iterator iter=vecWeight.begin();
		iter!=vecWeight.end(); ++iter)
	{
		sum += *iter;
	}
	if (0 == sum)
	{
		return 0;
	}

	uint32 randWeight = ::rand()%sum + 1;
	uint32 i=0;
	uint32 current_sum=0;
	for (std::vector<uint32>::const_iterator iter=vecWeight.begin();
		iter!=vecWeight.end(); ++iter, ++i)
	{	
		current_sum+=*iter;
		if(randWeight <= current_sum)
		{
			return i;
		}
	}
	return 0;
}



namespace wyl
{

	void InitSeed()
	{
		srand((unsigned int)time(0));
	}

	uint32 randBetween(uint32 min, uint32 max)
    {
        if(min == max)
            return min;
        else if(min > max){
            return max + rand32()%(min - max+1);
        }
        else{
            return min + rand32()%(max - min+1);
        }
    }

    uint32 rand32()
    {
        //确保 RAND_MAX == 0x7fff 才有效
        return (::rand()<<17)|(::rand()<<2)|(::rand());
    }

    int GetRandomNumber(int iMin, int iMax)
    {
        int i = 0;
        long long llTmp = 0;

        if(iMin > iMax)
        {
            std::swap(iMin,iMax); //反过来处理吧。
        }

        if (iMin == iMax)
        {
            return iMin;
        }
        else if (iMax <= RAND_MAX)
        {
            i = rand() % (iMax - iMin + 1) + iMin;
        }
        else
        {
            llTmp = (iMax + RAND_MAX - 1) * rand();
            i = (int)abs(llTmp % (iMax - iMin));
            i += iMin;
        }
        return i;
    }


    // 生成[low, high]之间的无重复的cnt个随机数
    bool GetIntervalNoRepeatedRandNum(U16 low, U16 high, U16 cnt, U16 *num)
    {
        if(low > high)
        {
            std::swap(low,high); //反过来处理
        }
        const U16 iTotal = high - low + 1;
        if (cnt > iTotal)
        {
            return false;
        }

        U16* piTemp = new U16[iTotal];
        if (!piTemp)
        {
            return false;
        }

        U16 iRange = iTotal;
        U16 iIdx;
        U16 i, j;

        for (i = 0; i < iTotal; i++)
        {
            piTemp[i] = i + low; // 随机数可能的取值
        }

        for (j = 0; j < cnt; j++)
        {
            iIdx = rand() % iRange;
            num[j] = piTemp[iIdx];
            piTemp[iIdx] = piTemp[iRange - 1];
            iRange--;
        }

        delete [] piTemp;
        return true;
    }
}//namespace wyl
