/*! \file G_Thwap.cpp
	\copyright Copyright (c) 2010 Sunside Inc., All Rights Reserved.
	\copyright See Crow/LICENSE for licensing terms.
*/

#include "G_Thwap.h"
#include <Engine/World/World.h>

namespace world {

G_Thwap::G_Thwap() : m_nextTick(0.f), m_touching(false)
{
}

G_Thwap::~G_Thwap()
{
}

void G_Thwap::Tick(int frame, float dt, const xtime::TimeSlice &time)
{
	Entity::Tick(frame, dt, time);

	if (m_nextTick > world->time)
		return; // not ready to think.

	BBox bbox(ps->bbox);
	bbox.Translate(ps->worldPos);

	BBox player(world->playerPawn->ps->bbox);
	player.Translate(world->playerPawn->ps->worldPos);

	if (bbox.Touches(player))
	{
		if (!m_touching)
		{
			if (PushEntityCall("OnPlayerTouched"))
				world->lua->Call("G_Thwap::Tick", 1, 0, 0);
		}

		m_touching = true;
	}
	else
	{
		m_touching = false;
	}

	m_nextTick = world->time + (50/1000.f);
}

} // world


namespace spawn {

void *x_thwap::Create()
{
	return new (ZWorld) world::G_Thwap();
}

void *y_thwap::Create()
{
	return new (ZWorld) world::G_Thwap();
}

void *z_thwap::Create()
{
	return new (ZWorld) world::G_Thwap();
}

} // spawn
