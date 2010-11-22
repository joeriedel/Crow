// G_Worldspawn.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#include <Engine/World/Entities/E_WorldSpawn.h>
#include <Runtime/PushPack.h>

namespace world {

class G_Worldspawn : public E_Worldspawn
{
public:
	G_Worldspawn();
	virtual ~G_Worldspawn();
};

} // world

E_DECL_SPAWN(RADNULL_API, worldspawn)

#include <Runtime/PopPack.h>
