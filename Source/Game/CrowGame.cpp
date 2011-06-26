// CrowGame.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#include "CrowGame.h"
#include <Engine/Game/GSLoadMap.h>
#include <Engine/Game/GSPlay.h>

Game::Tickable::Ref GSLoadMap::New(int mapId, int slot, bool play)
{
	return Game::Tickable::Ref(new (ZWorld) GSLoadMap(mapId, slot, play));
}

Game::Tickable::Ref GSPlay::New()
{
	return Game::Tickable::Ref(new (ZWorld) GSPlay());
}

Game::Ref Game::New()
{
	return Game::Ref(new CrowGame());
}

CrowGame::CrowGame()
{
}

bool CrowGame::LoadEntry()
{
	return LoadMapSeq("Cinematic/ToL", 1, world::UD_Slot, false);
}
