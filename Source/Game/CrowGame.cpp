// CrowGame.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#include "CrowGame.h"
#include <Engine/Game/GSLoadMap.h>
#include <Engine/Game/GSPlay.h>

Game::Tickable::Ref GSLoadMap::New(int mapId)
{
	return Game::Tickable::Ref(new (ZWorld) GSLoadMap(mapId));
}

Game::Tickable::Ref GSPlay::New()
{
	return Game::Tickable::Ref(new (ZWorld) GSPlay());
}

Game::Ref Game::New(App &app)
{
	return Game::Ref(new CrowGame(app));
}

CrowGame::CrowGame(App &app) : Game(app)
{
}
