// CrowGame.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#pragma once

#include <Engine/Game.h>

class Game : public GameBase
{
public:
	Game();
	virtual ~Game();

	virtual bool PreInit();
	virtual bool Initialize(const task::HControl &control);
	virtual void Finalize();

#if defined(RAD_OPT_PC)
	virtual void CloseWindow();
	virtual void FocusChange(bool focus);
#endif

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
};
