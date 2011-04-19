// G_ScreenView.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)
// See Radiance/LICENSE for licensing terms.

#include "G_ScreenView.h"
#include <algorithm>
#undef max

namespace world {

G_ScreenView::G_ScreenView() : 
E_CONSTRUCT_BASE,
m_pos(Vec3::Zero),
m_maxSpeed(Vec3::Zero),
m_velocity(Vec3::Zero),
m_accel(Vec3::Zero),
m_mins(Vec3::Zero),
m_maxs(Vec3::Zero),
m_screenMode(false)
{
}

G_ScreenView::~G_ScreenView()
{
}

void G_ScreenView::DoSyncLuaState(lua_State *L)
{
	Entity::DoSyncLuaState(L);

	lua_getfield(L, -1, "screenMins");
	m_mins = lua::Marshal<Vec3>::Get(L, -1, true);
	lua_pop(L, 1);

	lua_getfield(L, -1, "screenMaxs");
	m_maxs = lua::Marshal<Vec3>::Get(L, -1, true);
	lua_pop(L, 1);

	lua_getfield(L, -1, "screenMaxVelocity");
	m_maxSpeed = lua::Marshal<Vec3>::Get(L, -1, true);
	lua_pop(L, 1);

	lua_getfield(L, -1, "screenVelocity");
	m_velocity = lua::Marshal<Vec3>::Get(L, -1, true);
	lua_pop(L, 1);

	lua_getfield(L, -1, "screenAccel");
	m_accel = lua::Marshal<Vec3>::Get(L, -1, true);
	lua_pop(L, 1);

	lua_getfield(L, -1, "screenPos");
	m_pos = lua::Marshal<Vec3>::Get(L, -1, true);
	lua_pop(L, 1);

	lua_getfield(L, -1, "screenMode");
	m_screenMode = lua_toboolean(L, -1) ? true : false;
	lua_pop(L, 1);
}

void G_ScreenView::DoSetLuaState(lua_State *L)
{
	Entity::DoSetLuaState(L);

	lua::Marshal<Vec3>::Push(L, m_mins);
	lua_setfield(L, -2, "screenMins");
	lua::Marshal<Vec3>::Push(L, m_maxs);
	lua_setfield(L, -2, "screenMaxs");
	lua::Marshal<Vec3>::Push(L, m_maxSpeed);
	lua_setfield(L, -2, "screenMaxVelocity");
	lua::Marshal<Vec3>::Push(L, m_velocity);
	lua_setfield(L, -2, "screenVelocity");
	lua::Marshal<Vec3>::Push(L, m_accel);
	lua_setfield(L, -2, "screenAccel");
	lua::Marshal<Vec3>::Push(L, m_pos);
	lua_setfield(L, -2, "screenPos");
	lua_pushboolean(L, m_screenMode ? 1 : 0);
	lua_setfield(L, -2, "screenMode");
}

void G_ScreenView::TickPhysics(
	int frame, 
	float dt, 
	const xtime::TimeSlice &time
)
{
	if (!m_screenMode)
	{ // not in screen mode
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

	m_velocity += m_accel*dt;
	for (int i = 0; i < 3; ++i)
		m_velocity[i] = math::Clamp(m_velocity[i], -m_maxSpeed[i], m_maxSpeed[i]);

	m_pos += m_velocity*dt;
	for (int i = 0; i < 3; ++i)
		m_pos[i] = math::Clamp(m_pos[i], m_mins[i], m_maxs[i]);

	m_ps.worldAngles = WrapAngles(m_ps.originAngles + m_ps.angles);
	m_ps.cameraAngles = m_ps.worldAngles;

	Mat4 m = Mat4::Rotation(QuatFromAngles(Vec3(0, 0, m_ps.worldAngles[2])));
	Vec3 fwd = Vec3(1, 0, 0) * m;
	Vec3 up, left;
	FrameVecs(fwd, up, left);

	m_ps.pos = (fwd * m_pos[2]) + (left * m_pos[0]) + (up * m_pos[1]);
	m_ps.cameraPos = m_ps.origin;
	Move(true, true);
}

} // world
