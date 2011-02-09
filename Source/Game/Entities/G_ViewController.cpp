// G_ViewController.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#include "G_ViewController.h"
#include <Engine/World/World.h>

namespace world {

G_ViewController::G_ViewController()
{
}

G_ViewController::~G_ViewController()
{
}

void G_ViewController::PostSpawn()
{
	if (!world->playerPawn.get())
		return;
	this->pos = world->playerPawn->pos;
	world->camera->pos = this->pos;
}

} // world

namespace spawn {

void *view_controller::Create()
{
	return new (ZWorld) world::G_ViewController();
}

} // spawn
