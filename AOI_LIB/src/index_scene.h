/**
场景管理
待修改，ScreenIdx改为screen_idx

使用例子:
class MySceneActor : public SceneActor
{
...
}

class Player
{
	MySceneActor m_scene_actor;
	SYS1  m_sys1; //...其他子系统
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


//#define NineScreenSceneNoCheck  //定义了就删掉检查错误代码，速度更快

class NineScreenScene;
//场景actor模板
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
	//认为设置失败的情况有：相同位置，阻挡。
	virtual bool CanSetPos(const Pos &pos) const = 0;
	virtual void OnChangePos(const Pos &new_pos){};
	virtual void OnEnterScene(){};
	virtual void OnLeaveScene(){};
	virtual void AddViewActor(SceneActor  &other_actor) = 0; //other_actor 进入我视野
	virtual void RemoveViewActor(SceneActor  &other_actor) = 0; // other_actor 离开我视野
	virtual void UpdateActorPos(SceneActor  &update_actor) = 0; // update_actor位置变化

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

		//@brief	检查阻挡
		bool checkAndFindPos(SceneActor *actor, Pos &dstPos, const uint16 &width, const uint16 &height, const uint16 &range=10) const;
		bool checkAndFindPos(SceneActor *actor, Pos &dstPos, const uint16 &range=10) const;
};

//模板 ActorCB 原型例子：  void ActorCB(SceneActor &actor){} 或者函数对象
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
		void SynChangeScreenIdx(SceneActor &actor, ScreenIdx new_si, ScreenIdx old_si); //切换屏索引，同步actor相互信息

	private:
		//@brief 	所有物件的索引
		SetActor m_all; 

		//@brief 	单屏物件索引(set实现)
		typedef std::map<ScreenIdx, SetActor> ScreenIdx2SetActor;
		ScreenIdx2SetActor m_ScreenIdx_2_set; //m_ScreenIdx_index[屏幕索引] == 物件集合

		//@brief 	单屏物件索引(vector实现)
		typedef std::map<ScreenIdx, VecActor> ScreenIdx2VecActor;
		ScreenIdx2VecActor m_ScreenIdx_2_vec;//m_ScreenIdx_index_vec[屏幕索引] == 物件集合(VEC)

		//@brief 	位置物件索引(不分物件类型)
		typedef std::map<uint32, ListActor> Pos2ActorList;
		Pos2ActorList m_pos_2_list;  //m_pos_index[位置索引] ==物件集合
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


//@brief	检测出生点的合理性并给出一个随机且在指定范围的出生点
template<typename SceneActor>
bool SceneBlock<SceneActor>::checkAndFindPos(SceneActor *actor, Pos &dstPos, const uint16 &width, const uint16 &height, const uint16 &radius) const
{
	if ((width == 0 && height == 0) && !checkSceneBlock(dstPos, actor))
	{
		return true;
	}

	//@brief	保证随机取点不会到地图外
	dstPos.x = (dstPos.x > width / 2) ? (dstPos.x - width / 2) : 0;
	dstPos.y = (dstPos.y > height / 2) ? (dstPos.y - height / 2) : 0;
	dstPos.x += wyl::GetRandomNumber(1, width);
	dstPos.y += wyl::GetRandomNumber(0, height);

	//@brief	在半径范围内找到一个没有阻挡的出生点
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
