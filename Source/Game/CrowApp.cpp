// CrowApp.cpp
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#include "CrowApp.h"

App *App::New() { return new CrowApp(); }

CrowApp::CrowApp()
{
}

CrowApp::~CrowApp()
{
}

bool CrowApp::PreInit()
{
	COut(C_Info) << string::Shorten(title).c_str() << " built on " << __DATE__ << " @ " << __TIME__ << std::endl;
	COut(C_Info) << "PreInit..." << std::endl;
	if (!App::PreInit()) return false;
	return true;
}

bool CrowApp::Initialize(const task::HControl &control)
{
	COut(C_Info) << "Initializing..." << std::endl;
	if (!App::Initialize(control)) return false;
	return true;
}

const wchar_t *CrowApp::RAD_IMPLEMENT_GET(title)
{
	return L"Crow";
}

const wchar_t *CrowApp::RAD_IMPLEMENT_GET(company)
{
	return L"Sunside";
}

const wchar_t *CrowApp::RAD_IMPLEMENT_GET(website)
{
	return L"www.sunsidegames.com";
}

void CrowApp::Finalize()
{
	App::Finalize();
}

bool CrowApp::RAD_IMPLEMENT_GET(allowMultipleInstances)
{
	return false;
}

#if defined(RAD_OPT_PC)
void CrowApp::CloseWindow()
{
	exit = true;
}

void CrowApp::FocusChange(bool focus)
{
}
#endif

#if defined(RAD_OPT_PC_TOOLS)
bool CrowApp::RAD_IMPLEMENT_GET(wantEditor)
{
	return true;
}

void CrowApp::SwitchEditorMode(bool editor)
{
	engine->SwitchEditorMode(editor);
}

void CrowApp::EditorStart()
{
}
#endif

