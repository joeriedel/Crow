// G_PlayerPawn.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#pragma once

#include <Engine/World/Entities/E_PlayerPawn.h>
#include <Runtime/PushPack.h>

namespace world {

class RADENG_CLASS G_PlayerPawn : public E_PlayerPawn
{
public:
	G_PlayerPawn();
	virtual ~G_PlayerPawn();

	virtual int Spawn(
		const Keys &keys,
		const xtime::TimeSlice &time,
		int flags
	);
};

} // world

E_DECL_SPAWN(RADNULL_API, info_player_start)

#include <Runtime/PopPack.h>
