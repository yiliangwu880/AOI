/**
��������
���޸ģ�ScreenIdx��Ϊscreen_idx

ʹ������:
class MySceneActor : public SceneActor
{
...
}

class Player
{
	MySceneActor m_scene_actor;
	SYS1  m_sys1; //...������ϵͳ
}

class SceneMgr
{
	NineScreenScene m_nine_screen;
}
 */

#pragma once

#include <set>
#include <map>
#include "screen_index.h" 
#include "utility/stlBoost.h"
#include "utility/random.h"


//#define NineScreenSceneNoCheck  //�����˾�ɾ����������룬�ٶȸ���

class NineScreenScene;
//����actorģ��
class SceneActor
{
	friend class NineScreenScene;
public:
	SceneActor();
	~SceneActor();
	NineScreenScene *GetScene() const{ return m_scene; };
	const Pos  &getPos() const{ return m_pos; };
	ScreenIdx  GetScreenIdx() const;

private:
	//��Ϊ����ʧ�ܵ�����У���ͬλ�ã��赲��
	virtual bool CanSetPos(const Pos &pos) const = 0;
	virtual void OnChangePos(const Pos &new_pos){};
	virtual void OnEnterScene(){};
	virtual void OnLeaveScene(){};
	virtual void AddViewActor(SceneActor  &other_actor) = 0; //other_actor ��������Ұ
	virtual void RemoveViewActor(SceneActor  &other_actor) = 0; // other_actor �뿪����Ұ
	virtual void UpdateActorPos(SceneActor  &update_actor) = 0; // update_actorλ�ñ仯

private:
	void setPos(const Pos &pos);
	void SetScreenIdx(ScreenIdx idx);
	void SetScene(NineScreenScene *scene);

private:
	NineScreenScene *m_scene;
	Pos m_pos;
	ScreenIdx m_screen_idx;
};

template<class SceneActor>
class SceneBlock
{
public:
		bool checkSceneBlock(const Pos &pos, const SceneActor *actor) const;
		bool checkMapBlock(const Pos &pos, const SceneActor *actor) const;

		//@brief	����赲
		bool checkAndFindPos(SceneActor *actor, Pos &dstPos, const uint16 &width, const uint16 &height, const uint16 &range=10) const;
		bool checkAndFindPos(SceneActor *actor, Pos &dstPos, const uint16 &range=10) const;
};

//ģ�� ActorCB ԭ�����ӣ�  void ActorCB(SceneActor &actor){} ���ߺ�������
class NineScreenScene 
{
public:
	typedef std::vector<SceneActor *> VecActor;
	typedef std::set<SceneActor *> SetActor;
	typedef std::list<SceneActor *> ListActor;

	public:
		~NineScreenScene();

		bool AddActor(SceneActor &actor, const Pos &newPos);
		bool RemoveActor(SceneActor &actor);
		bool TryChangePos(SceneActor &actor, const Pos &newPos);

		const ListActor &GetActorByPos(const Pos& pos) const;
		uint32 GetActorNum() const;

		template<class ActorCB>
		void ForeachOfScreen(ScreenIdx si, ActorCB& callback);
		template<class ActorCB>
		void ForeachOfNine(ScreenIdx si, ActorCB &cb);
		template<class ActorCB>
		void ForeachOfDirect(ScreenIdx si, const int dir, ActorCB& cb);
		template<class ActorCB>
		void ForeachOfReverseDirect(ScreenIdx si, const int dir, ActorCB& cb);

	private:
		void SynAddView(SceneActor &actor);
		void SynRemoveView(SceneActor &actor, ScreenIdx si);
		void SynChangeScreenIdx(SceneActor &actor, ScreenIdx new_si, ScreenIdx old_si); //�л���������ͬ��actor�໥��Ϣ

	private:
		//@brief 	�������������
		SetActor m_all; 

		//@brief 	�����������(setʵ��)
		typedef std::map<ScreenIdx, SetActor> ScreenIdx2SetActor;
		ScreenIdx2SetActor m_ScreenIdx_2_set; //m_ScreenIdx_index[��Ļ����] == �������

		//@brief 	�����������(vectorʵ��)
		typedef std::map<ScreenIdx, VecActor> ScreenIdx2VecActor;
		ScreenIdx2VecActor m_ScreenIdx_2_vec;//m_ScreenIdx_index_vec[��Ļ����] == �������(VEC)

		//@brief 	λ���������(�����������)
		typedef std::map<uint32, ListActor> Pos2ActorList;
		Pos2ActorList m_pos_2_list;  //m_pos_index[λ������] ==�������
}; 





template<class ActorCB>
void NineScreenScene::ForeachOfScreen(ScreenIdx si, ActorCB &cb)
{
	VecActor &vec = m_ScreenIdx_2_vec[si];
	for (auto &actor : vec)
	{
		cb(*actor);
	}
}

template<class ActorCB>
void NineScreenScene::ForeachOfNine(ScreenIdx si, ActorCB &cb)
{
	const VecSI &ninescreen = ScreenIdxMgr::GetInstance().getNineScreen(si);
	for(VecSI::const_iterator it = ninescreen.begin(); it != ninescreen.end(); ++it)
	{
		this->ForeachOfScreen(*it, cb);
	}
}
template<class ActorCB>
void NineScreenScene::ForeachOfDirect(ScreenIdx si, const int dir, ActorCB &cb)
{
	const VecSI &directScreen = ScreenIdxMgr::GetInstance().getDirectScreen(si, dir);
	for(VecSI::const_iterator it = directScreen.begin(); it != directScreen.end(); ++it)
	{
		this->ForeachOfScreen(*it, cb);
	}
}
template<class ActorCB>
void NineScreenScene::ForeachOfReverseDirect(ScreenIdx si, const int dir, ActorCB &cb)
{
	const VecSI &reverseDirectScreen = ScreenIdxMgr::GetInstance().getReverseDirectScreen(si, dir);
	for(VecSI::const_iterator it = reverseDirectScreen.begin(); it != reverseDirectScreen.end(); ++it)
	{
		this->ForeachOfScreen(*it, cb);
	}
} 



template<typename SceneActor>
bool SceneBlock<SceneActor>::checkSceneBlock(const Pos &pos, const  SceneActor *actor) const
{
	return checkMapBlock(pos, actor);
}

template<typename SceneActor>
bool SceneBlock<SceneActor>::checkMapBlock(const Pos &pos, const  SceneActor *actor) const
{
	return checkMapBlock(pos, actor);
}


//@brief	��������ĺ����Բ�����һ���������ָ����Χ�ĳ�����
template<typename SceneActor>
bool SceneBlock<SceneActor>::checkAndFindPos(SceneActor *actor, Pos &dstPos, const uint16 &width, const uint16 &height, const uint16 &radius) const
{
	if ((width == 0 && height == 0) && !checkSceneBlock(dstPos, actor))
	{
		return true;
	}

	//@brief	��֤���ȡ�㲻�ᵽ��ͼ��
	dstPos.x = (dstPos.x > width / 2) ? (dstPos.x - width / 2) : 0;
	dstPos.y = (dstPos.y > height / 2) ? (dstPos.y - height / 2) : 0;
	dstPos.x += wyl::GetRandomNumber(1, width);
	dstPos.y += wyl::GetRandomNumber(0, height);

	//@brief	�ڰ뾶��Χ���ҵ�һ��û���赲�ĳ�����
	return checkAndFindPos(actor, dstPos, radius);
}

template<typename SceneActor>
bool SceneBlock<SceneActor>::checkAndFindPos(SceneActor *actor, Pos &dstPos, const uint16 &radius) const
{
	Pos tmpPos;
	for (int16 i = 1; i < radius; ++i)
	{
		for (uint32 dir = DirType_Up; dir < DirType_Wrong; ++dir)
		{
			tmpPos.x = dstPos.x + i * WALK_ADJUST[dir][0];
			tmpPos.y = dstPos.y + i * WALK_ADJUST[dir][1];
			if (!checkSceneBlock(tmpPos, actor))
			{
				dstPos = tmpPos;
				return true;
			}
		}
	}
	return false;
}
