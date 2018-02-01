
#include "index_scene.h"

SceneActor::SceneActor()
:m_scene(nullptr)
, m_screen_idx(0)
{

}


SceneActor::~SceneActor()
{
	if (nullptr != m_scene)
	{
		LOG_FATAL("非法删除，actor还没离开场景");
	}
}

ScreenIdx SceneActor::GetScreenIdx() const
{
	return m_screen_idx;
}


void SceneActor::setPos(const Pos &pos)
{
	m_pos = pos;
	OnChangePos(pos);
}

void SceneActor::SetScreenIdx(ScreenIdx idx)
{
	m_screen_idx = idx;
}

void SceneActor::SetScene(NineScreenScene *scene)
{
	if (nullptr != m_scene)
	{
		OnLeaveScene();
	}
	m_scene = scene;
	if (nullptr != m_scene)
	{
		OnEnterScene();
	}
}

//////////////////////////////////////////////////////////////////////////////

NineScreenScene::~NineScreenScene()
{
	if (!m_all.empty())
	{
		LOG_FATAL("场景释放时，有actor未退出场景");
	}
}

bool NineScreenScene::AddActor(SceneActor &actor, const Pos &newPos)
{
	if (nullptr != actor.GetScene())
	{
		LOG_ERROR(false && "已经添加");
		return false;
	}

	if (!actor.CanSetPos(newPos))
	{
		LOG_ERROR("插入actor失败 (%u, %u)",  newPos.x, newPos.y);
		return false;
	}

	//excute
	ScreenIdx newScreenIdx = newPos.GetPosI();
	actor.setPos(newPos);
	actor.SetScreenIdx(newScreenIdx);

	bool r = m_all.insert(&actor).second;	
	if (!r)
	{
		LOG_FATAL("重复加入actor");
	}
	r = m_ScreenIdx_2_set[newScreenIdx].insert(&actor).second;
	if (!r)
	{
		LOG_FATAL("重复加入actor");
	}
	m_ScreenIdx_2_vec[newScreenIdx].push_back(&actor);
	m_pos_2_list[newPos.hash()].push_back(&actor);


	actor.SetScene(this);

	//actor同步视野
	SynAddView(actor);

	return true;
}

void NineScreenScene::SynAddView(SceneActor &actor)
{
	auto cb = [&actor](SceneActor &other_actor)
	{
		if (&actor != &other_actor)
		{
			actor.AddViewActor(other_actor);
			other_actor.AddViewActor(actor);
		}
	};
	ForeachOfNine(actor.GetScreenIdx(), cb);
}

bool NineScreenScene::RemoveActor(SceneActor &actor)
{
	if (nullptr == actor.GetScene())
	{
		LOG_ERROR("移除actor不在场景");
		return false;
	}
	auto all_it = m_all.find(&actor);
	if (all_it == m_all.end())
	{
		LOG_ERROR("error para, can't find actor");
		return false;
	}
	ScreenIdx si = actor.GetScreenIdx();
	SetActor &set_actor = m_ScreenIdx_2_set[si];
	VecActor &vec_actor = m_ScreenIdx_2_vec[si];

	auto set_it = set_actor.find(&actor);
	if (set_it == set_actor.end())
	{
		LOG_ERROR("error para, can't find actor");
		return false;
	}
	auto ls_iter = m_pos_2_list.find(actor.getPos().hash());
	if (ls_iter == m_pos_2_list.end())
	{
		LOG_ERROR("error para, can't find actor");
		return false;
	}
	ListActor &ls_actor = ls_iter->second;
	bool r = wyl::VecFind(vec_actor, &actor);
	if (!r)
	{
		LOG_ERROR("error para, can't find actor");
		return false;
	}

	//excute
	set_actor.erase(set_it);
	wyl::VecRemove(vec_actor, &actor);
	ls_actor.remove(&actor);
	m_all.erase(all_it);

	actor.SetScene(nullptr);

	SynRemoveView(actor, si);
	return true;
}

void  NineScreenScene::SynRemoveView(SceneActor &actor, ScreenIdx si)
{
	auto cb = [&actor](SceneActor &other_actor)
	{
		if (&actor != &other_actor)
		{
			actor.RemoveViewActor(other_actor);
			other_actor.RemoveViewActor(actor);
		}
	};
	ForeachOfNine(actor.GetScreenIdx(), cb);
}

void NineScreenScene::SynChangeScreenIdx(SceneActor &actor, ScreenIdx new_si, ScreenIdx old_si)
{
	if (ScreenIdxMgr::GetInstance().checkTwoPosIInNine(old_si, new_si))
	{
		ScreenIdxMgr::DirType dir = ScreenIdxMgr::GetInstance().getScreenDirect(old_si, new_si);
		const VecSI &reverse_vec = ScreenIdxMgr::GetInstance().getReverseDirectScreen(old_si, dir);
		for (const auto &si : reverse_vec)
		{
			auto cb = [&actor](SceneActor &other_actor)
			{
				if (&actor != &other_actor)
				{
					actor.RemoveViewActor(other_actor);
					other_actor.RemoveViewActor(actor);
				}
			};
			ForeachOfScreen(si, cb);
		}

		const VecSI &vec = ScreenIdxMgr::GetInstance().getDirectScreen(new_si, dir);
		for (const auto &si : vec)
		{
			auto cb = [&actor](SceneActor &other_actor)
			{
				if (&actor != &other_actor)
				{
					actor.AddViewActor(other_actor);
					other_actor.AddViewActor(actor);
				}
			};
			ForeachOfScreen(si, cb);
		}
	}
	else
	{
		SynRemoveView(actor, old_si);
		SynAddView(actor);
	}
}

bool NineScreenScene::TryChangePos(SceneActor &actor, const Pos &newPos)
{
	if (nullptr == actor.GetScene())
	{
		LOG_ERROR("没有添加到场景");
		return false;
	}

	Pos old_pos = actor.getPos();
	ScreenIdx old_si = actor.GetScreenIdx();
	ScreenIdx new_si = newPos.GetPosI();

	SetActor &old_set_actor = m_ScreenIdx_2_set[old_si];
	auto old_set_actor_it = old_set_actor.find(&actor);
	if (old_set_actor_it == old_set_actor.end())
	{
		LOG_ERROR("没有找到屏索引");
		return false;
	}
	if (old_pos == newPos)
	{
		LOG_ERROR("位置相等");
		return false;
	}
	if (!actor.CanSetPos(newPos))
	{
		LOG_ERROR("设置位置失败");
		return false;
	}
	auto pos_2_ls_it = m_pos_2_list.find(old_pos.hash());
	if (pos_2_ls_it == m_pos_2_list.end())
	{
		LOG_ERROR("找不到m_pos_index， %d", old_pos.hash());
		return false;
	}

	//excute
	actor.setPos(newPos);

	pos_2_ls_it->second.remove(&actor);
	if (pos_2_ls_it->second.empty())
	{
		m_pos_2_list.erase(pos_2_ls_it);
	}
	m_pos_2_list[newPos.hash()].push_back(&actor);

	if (old_si != new_si)
	{
		old_set_actor.erase(old_set_actor_it);
		wyl::VecRemove(m_ScreenIdx_2_vec[old_si], &actor);
		m_ScreenIdx_2_set[new_si].insert(&actor);
		m_ScreenIdx_2_vec[new_si].push_back(&actor);
		actor.SetScreenIdx(new_si);

		SynChangeScreenIdx(actor, new_si, old_si);
	}
	else
	{
		auto cb = [&actor](SceneActor &other_actor)
		{
			if (&actor != &other_actor)
			{
				other_actor.UpdateActorPos(actor);
			}
		};
		ForeachOfNine(old_si, cb);
	}
	return true;
}

const NineScreenScene::ListActor &NineScreenScene::GetActorByPos(const Pos& pos) const
{
	static ListActor empty;
	auto iter = m_pos_2_list.find(pos.hash());
	if (iter == m_pos_2_list.end())
	{
		return empty;
	}
	return iter->second;
}


uint32 NineScreenScene::GetActorNum() const
{
	return m_all.size();
}

