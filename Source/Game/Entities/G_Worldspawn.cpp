// G_Worldspawn.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#include "G_Worldspawn.h"

namespace world {

G_Worldspawn::G_Worldspawn()
{
}

G_Worldspawn::~G_Worldspawn()
{
}

} // world

namespace spawn {

void *worldspawn::Create()
{
	return new (ZWorld) world::G_Worldspawn();
}

} // spawn
