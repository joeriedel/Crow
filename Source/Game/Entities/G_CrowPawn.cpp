// G_CrowPawn.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)
// See Radiance/LICENSE for licensing terms.

#include "G_CrowPawn.h"

namespace world {

G_CrowPawn::G_CrowPawn() : E_CONSTRUCT_BASE
{
}

G_CrowPawn::~G_CrowPawn()
{
}

} // world

namespace spawn {

void *info_player_start::Create()
{
	return new (ZWorld) world::G_CrowPawn();
}

} // spawn

