// CrowApp.cpp
// Copyright (c) 2010 Sunside Inc., All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#include "CrowApp.h"
#include <Engine/Engine.h>
#include <Engine/Utils/Tokenizer.h>
#include <Runtime/File.h>
#include "Entities/G_Exports.h"
#if defined(RAD_TARGET_GOLDEN) && defined(RAD_OPT_PC)
#include <Engine/Renderer/PC/RBackend.h>
#endif

#if defined(RAD_OPT_IOS)
bool __IOSAPP_AllowHD()
{
#if defined(SUPPORT_HD)
	return true;
#else
	return false;
#endif
}

#endif

App *App::New() { return new CrowApp(); }

CrowApp::CrowApp()
{
	spawn::G_Exports();
}

CrowApp::~CrowApp()
{
}

bool CrowApp::PreInit()
{
	COut(C_Info) << string::Shorten(title).c_str() << " built on " << __DATE__ << " @ " << __TIME__ << std::endl;
	COut(C_Info) << "PreInit..." << std::endl;
	if (!App::PreInit()) 
		return false;
	return true;
}

bool CrowApp::Initialize()
{
	COut(C_Info) << "Initializing..." << std::endl;
	if (!App::Initialize()) 
		return false;

#if defined(RAD_TARGET_GOLDEN)
	file::HFile file;

	int media = file::AllMedia;
	int r = engine->sys->files->OpenFile(
		L"pak0.pak",
		media,
		file,
		file::HIONotify()
	);

	if (r < file::Success)
	{
		COut(C_Error) << "Unable to open pak0.pak!" << std::endl;
		return false;
	}

	file::HPakFile pak = engine->sys->paks->MountPakFile(file, L"pak0.pak");
	if (!pak)
	{
		COut(C_Error) << "Unable to mount pak0.pak!" << std::endl;
		return false;
	}

	engine->sys->files->AddPakFile(pak);

	// disable all file access except through the pak file.
	engine->sys->files->enabledMedia = file::Paks;

	// bind context (pc only)
#if defined(RAD_OPT_PC)
	engine->sys->r->ctx = engine->sys->r.Cast<r::IRBackend>()->BindContext();
#endif
#endif

#if defined(RAD_TARGET_GOLDEN) || defined(RAD_OPT_IOS)
	return RunAutoExec();
#else
	return true;
#endif
}

#if defined(RAD_TARGET_GOLDEN) || defined(RAD_OPT_IOS)
bool CrowApp::RunAutoExec()
{
	wchar_t nativePath[file::MaxFilePathLen+1];
	file::ExpandToNativePath(L"9:/autoexec.txt", nativePath, file::MaxFilePathLen+1);
	FILE *fp = file::wfopen(nativePath, L"rb");
	if (!fp)
		return false;
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	void *data = safe_zone_malloc(ZEngine, size);
	if (fread(data, 1, size, fp) != size)
	{
		fclose(fp);
		zone_free(data);
		return false;
	}
	fclose(fp);

	Tokenizer script;
	script.InitParsing((const char *)data, (int)size);
	zone_free(data);

	String token;
	if (script.GetToken(token))
	{
		m_game = Game::New();
		r::VidMode vidMode = engine->sys->r->curVidMode;
		m_game->SetViewport(0, 0, vidMode.w, vidMode.h);
		if (!(m_game->LoadEntry() && m_game->LoadMapSeq(token.c_str(), 0, world::UD_Slot, true)))
		{
			m_game.reset();
			return false;
		}
	}

	return true;
}
#endif

void CrowApp::OnTick(float dt)
{
#if defined(RAD_TARGET_GOLDEN) || defined(RAD_OPT_IOS)
	if (m_game)
	{
		r::VidMode vidMode = engine->sys->r->curVidMode;
		m_game->SetViewport(0, 0, vidMode.w, vidMode.h);
		m_game->Tick(dt);
		engine->sys->r->SwapBuffers();
	}
#endif
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
#if defined(RAD_TARGET_GOLDEN) || defined(RAD_OPT_IOS)
	m_game.reset();
#if defined(RAD_OPT_PC)
	engine->sys->r->ctx = r::HContext();
#endif
#endif
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

