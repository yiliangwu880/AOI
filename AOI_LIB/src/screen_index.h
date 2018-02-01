/*
 @brief	������
 */

#pragma once

#include "screen_type.h"
#include <array>

namespace
{
	//X,Y�������
	const int WALK_ADJUST[9][2] = { { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, 0 } };
}

//@brief	��Ļ������ϵ������
class ScreenIdxMgr
{
private:
	ScreenIdxMgr();
	~ScreenIdxMgr(){};

public:	
	static ScreenIdxMgr &GetInstance();
	enum DirType
	{
		DirType_Up = 0,	/// ����
		DirType_RightUp = 1,	/// ����
		DirType_Right = 2,	/// ����
		DirType_RightDown = 3,	/// ����
		DirType_Down = 4,	/// ����
		DirType_LeftDown = 5,	/// ����
		DirType_Left = 6,	/// ����
		DirType_LeftUp = 7,	/// ����
		DirType_Wrong = 8,	/// ������
		DirType_Random = 8,	/// �������
	};
	//@brief	����������Ƿ��Ǿ�����ϵ
	/// @param	posIOne ��һ����
	/// @param	posITwo �ڶ�����
	/// @direct	���posIOne�ķ���
	/// @return	���posITwo��posIOne��direct��������һ��������true;���򷵻�false
	bool checkTwoPosIInNine(const ScreenIdx &posIOne, const ScreenIdx &posITwo, const uint8 &direct);
	//@brief	��������������Ƿ���9����ϵ
	/// @param	one ��һ�����
	/// @param	two �ڶ������
	/// @retturn	�Ƿ���9����ϵ
	bool checkTwoPosIInNine(const ScreenIdx one, const ScreenIdx two);
	//@brief	��������ŵõ���������
	/// @param	posi �����
	/// @return	����
	const VecSI &getNineScreen(const ScreenIdx &posi) const;
	//@brief ��������źͷ���õ�ǰ����
	/// @param posi �����
	/// @param direct ����
	/// @return ǰ����
	const VecSI &getDirectScreen(const ScreenIdx &posi, const int &direct) const;
	//@brief	��������źͷ���õ�������
	/// @param	posi �����
	/// @param	direct ����
	/// @return	������
	const VecSI &getReverseDirectScreen(const ScreenIdx &posi, const int &direct) const;
	//@brief	�����֮�����Է���
	/// @param	posiorg ��ʼ��
	/// @param	posinew ����һ��
	/// @return	����
	DirType getScreenDirect(const ScreenIdx posiorg, const ScreenIdx posinew);
	//@brief	��ӡ���з����ǰ����
	void printAllDirectScreen(const ScreenIdx &posi);
private:
	//@brief	��ʼ������ϵ
	/// @return	��ʼ���Ƿ�ɹ�
	bool initScreenIndex();
private:
	//@brief	��������
	std::array<VecSI, MAP_SCREEN_MAX> m_ninescreen; //m_ninescreen[��Ļ����] == PosIVector�����浱ǰ������<=9����Ļ
	//@brief	ǰ��������,��ά����[8][MAP_SCREEN_MAX]
	std::array<std::array<VecSI, MAP_SCREEN_MAX>, 8> m_direct_screen;//m_direct_screen[����][��Ļ����]  == ��Ļ�б�3������5����
	//@brief	����������
	std::array<std::array<VecSI, MAP_SCREEN_MAX>, 8> m_reverse_direct_screen; //m_reverse_direct_screen[����][��Ļ����]  == ��Ļ�б�3������5����
};
