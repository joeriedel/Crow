// G_ScreenPawn.cpp
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)
// See Radiance/LICENSE for licensing terms.

#include "G_ScreenPawn.h"

namespace world {

G_ScreenPawn::G_ScreenPawn() : E_CONSTRUCT_BASE
{
}

G_ScreenPawn::~G_ScreenPawn()
{
}

} // world

namespace spawn {

void *info_player_start::Create()
{
	return new (ZWorld) world::G_ScreenPawn();
}

void *info_enemy_start::Create()
{
	return new (ZWorld) world::G_ScreenPawn();
}

} // spawn