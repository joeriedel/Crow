// G_PlayerPawn.cpp
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#include "G_PlayerPawn.h"
#include <Engine/World/World.h>

namespace world {

G_PlayerPawn::G_PlayerPawn()
{
}

G_PlayerPawn::~G_PlayerPawn()
{
}

int G_PlayerPawn::Spawn(
	const Keys &keys,
	const xtime::TimeSlice &time,
	int flags
)
{
	world->playerPawn = shared_from_this();
	this->org = keys.Vec3ForKey("origin");
	return pkg::SR_Success;
}

} // world

namespace spawn {

void *info_player_start::Create()
{
	return new (ZWorld) world::G_PlayerPawn();
}

} // spawn
