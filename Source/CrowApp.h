// CrowApp.h
// Copyright (c) 2010 Sunside Inc., All Rights Reserved
// Author: Joe Riedel
// See Crow/LICENSE for licensing terms.

#pragma once

#include <Engine/App.h>

#if defined(RAD_TARGET_GOLDEN) || defined(RAD_OPT_IOS)
#include <Engine/Game/Game.h>
#endif

class CrowApp : public App {
public:
	CrowApp(int argc, const char **argv);
	virtual ~CrowApp();

	virtual bool PreInit();
	virtual bool InitWindow();
	virtual bool Initialize();
	virtual void Finalize();
	virtual int DoLauncher();
	virtual bool Run();
	virtual void NotifyBackground(bool background);
	virtual void PostInputEvent(const InputEvent &e);

protected:

	virtual void OnTick(float dt);

private:

	virtual RAD_DECLARE_GET(title, const char*);
	virtual RAD_DECLARE_GET(company, const char*);
	virtual RAD_DECLARE_GET(website, const char*);
	virtual RAD_DECLARE_GET(flurryAPIKey, const char*);

#if defined(RAD_TARGET_GOLDEN) || defined(RAD_OPT_IOS)
	virtual RAD_DECLARE_GET(game, Game*) { return m_game.get(); }
	bool RunAutoExec();
	Game::Ref m_game;
#endif

	bool m_background;
};
