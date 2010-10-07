// CrowGame.cpp
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#include "CrowGame.h"

GameBase *GameBase::New() { return new Game(); }

Game::Game()
{
}

Game::~Game()
{
}

bool Game::PreInit()
{
	COut(C_Info) << string::Shorten(title).c_str() << " built on " << __DATE__ << " @ " << __TIME__ << std::endl;
	COut(C_Info) << "PreInit..." << std::endl;
	if (!GameBase::PreInit()) return false;
	return true;
}

bool Game::Initialize(const task::HControl &control)
{
	COut(C_Info) << "Initializing..." << std::endl;
	if (!GameBase::Initialize(control)) return false;
	return true;
}

const wchar_t *Game::RAD_IMPLEMENT_GET(title)
{
	return L"Crow";
}

const wchar_t *Game::RAD_IMPLEMENT_GET(company)
{
	return L"Sunside";
}

const wchar_t *Game::RAD_IMPLEMENT_GET(website)
{
	return L"www.sunsidegames.com";
}

void Game::Finalize()
{
	GameBase::Finalize();
}

bool Game::RAD_IMPLEMENT_GET(allowMultipleInstances)
{
	return false;
}

#if defined(RAD_OPT_PC)
void Game::CloseWindow()
{
	exit = true;
}

void Game::FocusChange(bool focus)
{
}
#endif

#if defined(RAD_OPT_PC_TOOLS)
bool Game::RAD_IMPLEMENT_GET(wantEditor)
{
	return true;
}

void Game::SwitchEditorMode(bool editor)
{
	engine->SwitchEditorMode(editor);
}

void Game::EditorStart()
{
}
#endif

