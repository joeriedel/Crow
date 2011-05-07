// G_ScreenView.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)
// See Radiance/LICENSE for licensing terms.

#include "G_ScreenView.h"
#include <Engine/World/World.h>
#include <algorithm>
#undef max

namespace world {

G_ScreenView::G_ScreenView() : 
E_CONSTRUCT_BASE,
m_enabled(false),
m_target(Vec3::Zero),
m_targetVel(Vec3::Zero),
m_clip(Vec3::Zero)
{
}

G_ScreenView::~G_ScreenView()
{
}

void G_ScreenView::DoSetLuaState(lua_State *L)
{
	Entity::DoSetLuaState(L);

	lua_createtable(L, 0, 3);
	lua_pushboolean(L, m_enabled ? 1 : 0);
	lua_setfield(L, -2, "enabled");
	lua::Marshal<physics::Spring>::Push(L, m_spring);
	lua_setfield(L, -2, "spring");
	lua::Marshal<physics::SpringVertex>::Push(L, m_vertex);
	lua_setfield(L, -2, "vertex");
	lua::Marshal<Vec3>::Push(L, m_target);
	lua_setfield(L, -2, "target");
	lua::Marshal<Vec3>::Push(L, m_targetVel);
	lua_setfield(L, -2, "targetVel");
	lua::Marshal<Vec3>::Push(L, m_clip);
	lua_setfield(L, -2, "clip");
	lua_setfield(L, -2, "screenControl");

}

void G_ScreenView::DoSyncLuaState(lua_State *L)
{
	Entity::DoSyncLuaState(L);

	lua_getfield(L, -1, "screenControl");
	lua_getfield(L, -1, "enabled");
	m_enabled = lua_toboolean(L, -1) ? true : false;
	lua_pop(L, 1);
	lua_getfield(L, -1, "spring");
	m_spring = lua::Marshal<physics::Spring>::Get(L, -1, false);
	lua_pop(L, 1);
	lua_getfield(L, -1, "vertex");
	m_vertex = lua::Marshal<physics::SpringVertex>::Get(L, -1, false);
	lua_pop(L, 1);
	lua_getfield(L, -1, "target");
	m_target = lua::Marshal<Vec3>::Get(L, -1, false);
	lua_pop(L, 1);
	lua_getfield(L, -1, "targetVel");
	m_targetVel = lua::Marshal<Vec3>::Get(L, -1, false);
	lua_pop(L, 1);
	lua_getfield(L, -1, "clip");
	m_clip = lua::Marshal<Vec3>::Get(L, -1, false);
	lua_pop(L, 2);
}

void G_ScreenView::TickPhysics(
	int frame, 
	float dt, 
	const xtime::TimeSlice &time
)
{
	if (!m_enabled)
	{ // no screen controls
		Entity::TickPhysics(frame, dt, time);
		return;
	}

	Entity::Ref parent = m_ps.parent.lock();
	if (parent)
	{
		TickOther(*parent, frame, dt, time);
		m_ps.origin = parent->ps->worldPos;
		m_ps.originAngles = parent->ps->worldAngles;
	}

	m_target += m_targetVel*dt;
	for (int i = 0; i < 3; ++i)
		m_target[i] = math::Clamp(m_target[i], -m_clip[i], m_clip[i]);

	m_vertex.Update(dt, m_target, m_spring);

	m_ps.worldAngles = WrapAngles(m_ps.originAngles + m_ps.angles.pos);
	m_ps.cameraAngles = m_ps.worldAngles;

	Mat4 m = Mat4::Rotation(QuatFromAngles(Vec3(0, 0, m_ps.worldAngles[2])));
	Vec3 fwd = Vec3(1, 0, 0) * m;
	Vec3 up, left;
	FrameVecs(fwd, up, left);

	m_ps.pos = (left*m_vertex.pos[0])+(up*m_vertex.pos[1])+(fwd*m_vertex.pos[2]);
	m_ps.cameraPos = m_ps.origin;
	Move(true, true);
}

} // world
