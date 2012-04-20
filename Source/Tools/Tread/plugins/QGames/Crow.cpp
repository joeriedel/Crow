// Crow.cpp
// Copyright (c) 2010 Sunside Inc., All Rights Reserved
// Author: Joe Riedel
// See Radiance/LICENSE for licensing terms.

#include "stdafx.h"
#include "Crow.h"
#include "splinetrack.h"

///////////////////////////////////////////////////////////////////////////////
// CrowGame
///////////////////////////////////////////////////////////////////////////////

const char *CrowGame::Name()
{
	return "Crow";
}

void CrowGame::GetExportFile(const char *filename, char *buff, int buffSize)
{
	strcpy(buff, GameDir());
	strcat(buff, "\\Base\\Maps\\");
	strcat(buff, filename);
	StrSetFileExtension(buff, "map");
}

CPluginFileExport *CrowGame::NativeMapExporter()
{
	return new CrowMap();
}

CPluginFileExport *CrowGame::FileExporter(int i)
{
	switch (i)
	{
	case 0: return new CrowMap();
	}
	return 0;
}

const char *CrowGame::PakType(int i)
{
	return 0;
}

CTextureFactory *CrowGame::CreatePakFile()
{
	return 0;
}

CQuakeUserData *CrowGame::CreateUserData(CTreadDoc *doc)
{
	return new CrowUserData(doc, this);
}

void CrowGame::InitializeToolsList(QuakeToolsList &tools)
{
}

void CrowGame::RunMapCompile(const char *inpath, QuakeToolsList &tools, CTreadDoc *doc, bool runGame)
{
	CQuakeGame::RunMapCompile(inpath, tools, doc, runGame);
}

void CrowGame::GetLeakFileName(CTreadDoc *doc, char *buff, int buffSize)
{
	buff[0] = 0;
}

CQuakeCompiler *CrowGame::CreateCompiler(const char *filename, QuakeToolsList &tools, CTreadDoc *doc, bool runMap)
{
	return new CQuakeCompiler(filename, tools, doc, runMap);
}

CObjectCreator *CrowGame::ObjectCreator(int i)
{
	if (i < 3)
		return CQuakeGame::ObjectCreator(i);
	switch (i)
	{
	case 3:
		return CSplineTrack::Creator();
		break;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// CrowUserData
///////////////////////////////////////////////////////////////////////////////

void CrowUserData::MakeBrushMenu(CTreadDoc *doc)
{
	if( m_BrushMenuCreated )
		return;

	m_BrushMenu.AddMenuItem( BM_EDIT_FACES, "Edit Faces" );
	m_BrushMenu.AddMenuItem( BM_EDIT_VERTS, "Edit Verts" );
	m_BrushMenu.AddMenuItem( BM_LOCK_TEXTURE, "Lock Texture" );
	m_BrushMenu.AddMenuItem( BM_GRAB_TEXTURE, "Grab Texture" );
	m_BrushMenu.AddMenuItem( BM_APPLY_TEXTURE, "Apply Texture" );
	m_BrushMenu.AddMenuItem( BM_SELECT_ALL_WITH_TEXTURE, "Select All w/ Texture" );
//	m_BrushMenu.AddMenuItem( 15, "Edit Shader" );
	//m_BrushMenu.AddMenuItem( 7, "Mapping\nPlanar X");
	//m_BrushMenu.AddMenuItem( 8, "Mapping\nPlanar Y");
	//m_BrushMenu.AddMenuItem( 6, "Mapping\nPlanar Z");
	//m_BrushMenu.AddMenuItem( 9, "Mapping\nPlanar Auto");
	m_BrushMenu.AddMenuItem( BM_SNAP_TO_GRID, "Snap To Grid" );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( BM_CSG_HOLLOW, "Hollow..." );
	m_BrushMenu.AddMenuItem( BM_CSG_CARVE, "Carve" );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
#if 0
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_NODRAW, "Brush Filter\nNo Draw", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_SOLID, "Brush Filter\nSolid", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_WINDOW, "Brush Filter\nWindow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_WATER, "Brush Filter\nWater", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_AREAPORTAL, "Brush Filter\nAreaportal", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_DETAIL, "Brush Filter\nDetail", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_NOSHADOW, "Brush Filter\nNo Shadow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_ALWAYSSHADOW, "Brush Filter\nAlways Shadow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_MONSTECLIP, "Brush Filter\nMonster Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_PLAYERCLIP, "Brush Filter\nPlayer Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_CAMERACLIP, "Brush Filter\nCamera Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_CORONABLOCK, "Brush Filter\nCorona Block", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_SKYBOX, "Brush Filter\nSky Box", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_SKYPORTAL, "Brush Filter\nSky Portal", true );
	m_BrushMenu.AddMenuItem( 0, "Brush Filter\n@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_FILTERALL, "Brush Filter\nFilter All", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_FILTERNONE, "Brush Filter\nFilter None", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_FILTERALLCLIP, "Brush Filter\nFilter All Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_FILTERALLSKY, "Brush Filter\nFilter All Sky", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_BRUSHFILTER_REAPPLYFILTER, "Brush Filter\nRe-Apply Filter", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_NODRAW, "Select Brushes By Attributes\nNo Draw", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SOLID, "Select Brushes By Attributes\nSolid", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_WINDOW, "Select Brushes By Attributes\nWindow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_WATER, "Select Brushes By Attributes\nWater", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_AREAPORTAL, "Select Brushes By Attributes\nAreaportal", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_DETAIL, "Select Brushes By Attributes\nDetail", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_NOSHADOW, "Select Brushes By Attributes\nNo Shadow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_ALWAYSSHADOW, "Select Brushes By Attributes\nAlways Shadow", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_MONSTERCLIP, "Select Brushes By Attributes\nMonster Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_PLAYERCLIP, "Select Brushes By Attributes\nPlayer Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_CAMERACLIP, "Select Brushes By Attributes\nCamera Clip", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_CORONABLOCK, "Select Brushes By Attributes\nCorona Block", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SKYBOX, "Select Brushes By Attributes\nSky Box", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTBRUSHESBYATTRIBUTES_SKYPORTAL, "Select Brushes By Attributes\nSky Portal", true );
#endif
	m_BrushMenu.AddMenuItem( BM_SELECT_BRUSHES_WITH_SAME_CONTENTS, "Select Brushes With Same Contents" );
	m_BrushMenu.AddMenuItem( BM_SELECT_BRUSHES_WITH_SAME_SURFACE, "Select Brushes With Same Surface Values" );

	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_CHECKSELECTEDOBJECTS, "Check Selected Objects For Errors...", true );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );

	//
	// add link to entity objects.
	//
	CLinkedList<CEntDef>* entdefs = doc->GameDef()->EntDefList();
	CEntDef* def;
	int id = BM_FIRST_LINK_TO_ENTITY_MENU;

	for( def = entdefs->ResetPos(); def; def = entdefs->GetNextItem() )
	{
		if( def->IsOwner() && !def->IsBaseClass() )
		{
			CString s;

			s.Format( "Link To Entity\n%s", def->GetTreeDisplayName());
			m_BrushMenu.AddMenuItem( id++, s );
		}
	}

	m_BrushMenu.AddMenuItem( BM_UNLINK_FROM_ENTITY, "Unlink From Entity" );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_HIDE, "Hide Selected", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SHOWALL, "Show All", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_HIDEALL, "Hide All" , true );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_MAKEGROUP, "Make Group", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_MAKEGROUPANDHIDE, "Make Group and Hide", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_HIDEALLINGROUP, "Hide All In Group(s)", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SHOWALLINGROUPS, "Show All In Group(s)", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_SELECTALLINGROUPS, "Select All In Group(s)", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_REMOVEFROMGROUPS, "Remove From Group(s)", true );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_FLIPVERTICAL, "Flip Vertical", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_FLIPHORIZONTAL, "Flip Horizontal", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_FLIPONXAXIS, "Flip On X Axis", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_FLIPONYAXIS, "Flip On Y Axis", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_FLIPONZAXIS, "Flip On Z Axis", true );
	m_BrushMenu.AddMenuItem( 0, "@SEP@" );
	m_BrushMenu.AddMenuItem( ID_TOOLS_ROTATE90CW, "Rotate 90� CW", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_ROTATE90CCW, "Rotate 90� CCW", true );
	m_BrushMenu.AddMenuItem( ID_TOOLS_ROTATE180, "Rotate 180�", true );

	m_BrushMenuCreated = true;
}