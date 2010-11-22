// Crow.h
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)
// See Radiance/LICENSE for licensing terms.

#pragma once

#include "Quake.h"

///////////////////////////////////////////////////////////////////////////////
// CrowGame
///////////////////////////////////////////////////////////////////////////////

class CrowGame : public CQuakeGame
{
public:

	CrowGame() : CQuakeGame(CQBrush::TYPE_Q3) {}

	virtual const char *Name();

	virtual void GetExportFile(const char *filename, char *buff, int buffSize);
	virtual CPluginFileExport *NativeMapExporter();
	virtual CPluginFileExport *FileExporter(int i);

	virtual const char *PakType(int i);
	virtual CTextureFactory *CreatePakFile();
	virtual CQuakeCompiler *CreateCompiler(const char *filename, QuakeToolsList &tools, CTreadDoc *doc, bool runMap);

	virtual CQuakeUserData *CreateUserData(CTreadDoc *doc);
	virtual void InitializeToolsList(QuakeToolsList &tools);
	virtual void GetLeakFileName(CTreadDoc *doc, char *buff, int buffSize);

protected:

	virtual void RunMapCompile(const char *mappath, QuakeToolsList &tools, CTreadDoc *doc, bool runGame);

};

///////////////////////////////////////////////////////////////////////////////
// CrowUserData
///////////////////////////////////////////////////////////////////////////////

class CrowUserData : public CQuakeUserData
{
public:
	CrowUserData(CTreadDoc *doc, CrowGame *game) : CQuakeUserData(doc, game) {}
	void MakeBrushMenu(CTreadDoc *doc);
};

///////////////////////////////////////////////////////////////////////////////
// CrowMap
///////////////////////////////////////////////////////////////////////////////

class CrowMap : public CQuakeMap
{
public:

	virtual const char *Type() { return "Crow Map"; }
	virtual const char *Extension() { return "map"; }
	virtual const char *PluginGame() { return "Crow"; }

protected:

	virtual bool ExportTextures(const char *filename, CTreadDoc *doc) { return true; }
};


