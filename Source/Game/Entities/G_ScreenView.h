// G_ScreenView.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)
// See Radiance/LICENSE for licensing terms.

// A ScreenView entity is one that can be put into
// a mode where it's motion and position is constrained to
// the screen bounds, and can strafe left/right up/down

#pragma once

#include <Engine/World/Entity.h>
#include <Runtime/PushPack.h>

namespace world {

class G_ScreenView : public Entity
{
	E_DECL_BASE(Entity);
public:

	G_ScreenView();
	virtual ~G_ScreenView();

	RAD_DECLARE_PROPERTY(G_ScreenView, screenMins, const Vec3&, const Vec3&);
	RAD_DECLARE_PROPERTY(G_ScreenView, screenMaxs, const Vec3&, const Vec3&);
	RAD_DECLARE_PROPERTY(G_ScreenView, screenMaxSpeed, const Vec3&, const Vec3&);
	RAD_DECLARE_PROPERTY(G_ScreenView, screenVelocity, const Vec3&, const Vec3&);
	RAD_DECLARE_PROPERTY(G_ScreenView, screenAccel, const Vec3&, const Vec3&);
	RAD_DECLARE_PROPERTY(G_ScreenView, screenPos, const Vec3&, const Vec3&);

protected:

	virtual void DoSyncLuaState(lua_State *L);
	virtual void DoSetLuaState(lua_State *L);
	
	virtual void TickPhysics(
		int frame, 
		float dt, 
		const xtime::TimeSlice &time
	);

private:

	RAD_DECLARE_GET(screenMins, const Vec3&) { return m_mins; }
	RAD_DECLARE_SET(screenMins, const Vec3&) { m_mins = value; }
	RAD_DECLARE_GET(screenMaxs, const Vec3&) { return m_maxs; }
	RAD_DECLARE_SET(screenMaxs, const Vec3&) { m_maxs = value; }
	RAD_DECLARE_GET(screenMaxSpeed, const Vec3&) { return m_maxSpeed; }
	RAD_DECLARE_SET(screenMaxSpeed, const Vec3&) { m_maxSpeed = value; }
	RAD_DECLARE_GET(screenVelocity, const Vec3&) { return m_velocity; }
	RAD_DECLARE_SET(screenVelocity, const Vec3&) { m_velocity = value; }
	RAD_DECLARE_GET(screenAccel, const Vec3&) { return m_accel; }
	RAD_DECLARE_SET(screenAccel, const Vec3&) { m_accel = value; }
	RAD_DECLARE_GET(screenPos, const Vec3&) { return m_pos; }
	RAD_DECLARE_SET(screenPos, const Vec3&) { m_pos = value; }

	Vec3 m_pos;
	Vec3 m_maxSpeed;
	Vec3 m_velocity;
	Vec3 m_accel;
	Vec3 m_mins, m_maxs;
	bool m_screenMode;
};

} // world

#include <Runtime/PopPack.h>

