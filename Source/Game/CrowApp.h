// CrowApp.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#pragma once

#include <Engine/App.h>

#if defined(RAD_TARGET_GOLDEN) || defined(RAD_OPT_IOS)
#include <Engine/Game/Game.h>
#endif

class CrowApp : public App
{
public:
	CrowApp();
	virtual ~CrowApp();

	virtual bool PreInit();
	virtual bool Initialize();
	virtual void Finalize();

#if defined(RAD_OPT_PC)
	virtual void CloseWindow();
	virtual void FocusChange(bool focus);
#endif

protected:

	virtual void OnTick(float dt);

private:

	virtual RAD_DECLARE_GET(title, const wchar_t*);
	virtual RAD_DECLARE_GET(company, const wchar_t*);
	virtual RAD_DECLARE_GET(website, const wchar_t*);
	virtual RAD_DECLARE_GET(allowMultipleInstances, bool);

#if defined(RAD_OPT_PC_TOOLS)
	virtual RAD_DECLARE_GET(wantEditor, bool);
	virtual void SwitchEditorMode(bool editor);
	virtual void EditorStart();
#endif

#if defined(RAD_TARGET_GOLDEN) || defined(RAD_OPT_IOS)
	virtual RAD_DECLARE_GET(game, Game*) { return m_game.get(); }
	bool RunAutoExec();
	Game::Ref m_game;
#endif
};
