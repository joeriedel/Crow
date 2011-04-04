// G_CrowPawn.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)
// See Radiance/LICENSE for licensing terms.

#pragma once

#include "G_ScreenView.h"
#include <Runtime/PushPack.h>

namespace world {

class G_CrowPawn : public G_ScreenView
{
	E_DECL_BASE(G_ScreenView);
public:

	G_CrowPawn();
	virtual ~G_CrowPawn();
};

} // world

E_DECL_SPAWN(RADNULL_API, info_player_start)

#include <Runtime/PopPack.h>
