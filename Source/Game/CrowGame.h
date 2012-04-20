// CrowGame.h
// Copyright (c) 2010 Sunside Inc., All Rights Reserved
// Author: Joe Riedel
// See Crow/LICENSE for licensing terms.

#pragma once

#include <Engine/Game/Game.h>
#include <Runtime/PushPack.h>

class CrowGame : public Game
{
public:

	CrowGame();

	virtual bool LoadEntry();
};

#include <Runtime/PopPack.h>
