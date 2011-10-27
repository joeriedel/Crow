// CrowGame.h
// Copyright (c) 2010 Sunside Inc., All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)

#include "CrowGame.h"
#include <Engine/App.h>
#include <Engine/Engine.h>
#include <Engine/Game/GSLoadMap.h>
#include <Engine/Game/GSPlay.h>
#include <Engine/Renderer/TextModel.h>
#include <Engine/Packages/Packages.h>
#include <Engine/Assets/MaterialParser.h>
#include <Engine/Renderer/GL/GLState.h>
#include <Engine/Renderer/GL/GLTable.h>
#include <Engine/Renderer/GL/GLVertexBuffer.h>

using namespace r;

struct OverlayVert
{
	float xy[2];
	float st[2];
};

enum
{
	BaseRectSize = 64,
	OverlayDiv = 8
};

class GSCrowLoadMap : public GSLoadMap
{
public:
	GSCrowLoadMap(int mapId, int slot, bool play, bool loadScreen) : 
	  GSLoadMap(mapId, slot, play, loadScreen), m_elapsed(0.f), m_time(0.f), m_first(true)
	{
	}

	virtual void Draw(Game &game, float dt)
	{
		if (!loadScreen)
			return;
		if (m_first)
		{
			m_first = false;
			Load();
			mapAsset->world->draw->rb->ClearBackBuffer();
#if defined(RAD_OPT_PC_TOOLS)
			if (game.toolsCallback)
				game.toolsCallback->SwapBuffers();
			else
#endif
				App::Get()->engine->sys->r->SwapBuffers();

			mapAsset->world->draw->rb->ClearBackBuffer();

		}
		else
		{
			m_elapsed += dt;
		}

		Draw(game);
		
		m_elapsed = 0.f;
	}

private:

	void Load()
	{
		m_mat = App::Get()->engine->sys->packages->Resolve("UI/finger_shadow_M", pkg::Z_Engine);
		if (!m_mat)
			return;
		int r = m_mat->Process(
			xtime::TimeSlice::Infinite,
			pkg::P_Load
		);

		if (r < pkg::SR_Success)
			return;

		m_mat->Process(
			xtime::TimeSlice::Infinite,
			pkg::P_Trim
		);

		m_parser = asset::MaterialParser::Cast(m_mat);
		m_loader = asset::MaterialLoader::Cast(m_mat);

		InitRectVerts(BaseRectSize, BaseRectSize);
	}

	void Draw(Game &game)
	{
		if (!m_mat)
			return;
		
		int vpx, vpy, vpw, vph;
		mapAsset->world->game->Viewport(vpx, vpy, vpw, vph);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		gl.MatrixMode(GL_PROJECTION);
		gl.LoadIdentity();
		
		float yaspect = ((float)vph)/((float)vpw);

		gl.Ortho((double)vpx, (double)vpw, (double)(vpy+vph), (double)vpy, -1.0, 1.0);
		gl.MatrixMode(GL_MODELVIEW);
		gl.LoadIdentity();
		const float size = 128.f;
		gl.Translatef(vpw-size-8.f, vph-(size*yaspect)-8.f, 0.f);
		gl.Scalef(size/BaseRectSize, (size*yaspect)/BaseRectSize, 1.f);

		Material *m = m_parser->material;

		m->BindStates();
		m->BindTextures(m_loader);
		m->shader->Begin(r::Shader::P_Default, *m);

		gls.DisableAllMGSources();
		gls.SetMGSource(
			r::MGS_Vertices,
			0,
			m_rectVB,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(OverlayVert),
			0
		);
		gls.SetMGSource(
			r::MGS_TexCoords,
			0,
			m_rectVB,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(OverlayVert),
			sizeof(float)*2
		);
		gls.BindBuffer(
			GL_ELEMENT_ARRAY_BUFFER_ARB,
			m_rectIB
		);

		m->shader->BindStates(true, Vec4(1,1,1,1));
		gls.Commit();
		gl.DrawElements(GL_TRIANGLES, (OverlayDiv-1)*(OverlayDiv-1)*6, GL_UNSIGNED_SHORT, 0);
		CHECK_GL_ERRORS();
		m->shader->End();

		m_time += 0.1f;
		m->Sample(m_time, 0.1f);
	}
	
	void InitRectVerts(int vpw, int vph)
	{
		m_rectVB.reset(
		   new GLVertexBuffer(
			  GL_ARRAY_BUFFER_ARB, 
			  GL_STATIC_DRAW_ARB, 
			  sizeof(OverlayVert)*OverlayDiv*OverlayDiv
			)
	   );
		
		RAD_ASSERT(m_rectVB);
		
		GLVertexBuffer::Ptr::Ref vb = m_rectVB->Map();
		RAD_ASSERT(vb);
		OverlayVert *verts = (OverlayVert*)vb->ptr.get();
		
		float xInc = vpw / ((float)OverlayDiv-1);
		float yInc = vph / ((float)OverlayDiv-1);
		
		int x, y;
		float xf, yf;
		
		for (y = 0, yf = 0.f; y < OverlayDiv; ++y, yf += yInc)
		{
			for (x = 0, xf = 0.f; x < OverlayDiv; ++x, xf += xInc)
			{
				OverlayVert &v = verts[y*OverlayDiv+x];
				v.xy[0] = xf;
				v.xy[1] = yf;
				v.st[0] = xf / vpw;
				v.st[1] = yf / vph;
			}
		}
		
		vb.reset(); // unmap
		
		// setup triangle indices
		
		m_rectIB.reset(
		   new GLVertexBuffer(
			  GL_ELEMENT_ARRAY_BUFFER_ARB,
			  GL_STATIC_DRAW_ARB,
			  sizeof(U16)*(OverlayDiv-1)*(OverlayDiv-1)*6
			)
		);
		
		vb = m_rectIB->Map();
		RAD_ASSERT(vb);
		U16 *indices = (U16*)vb->ptr.get();
		
		for (y = 0; y < OverlayDiv-1; ++y)
		{
			for (x = 0; x < OverlayDiv-1; ++x)
			{
				U16 *idx = &indices[y*(OverlayDiv-1)*6+x*6];
				
				// glOrtho() inverts the +Z axis (or -Z can't recall), inverting the 
				// dot product sign and culling CCW faces (i.e. we're looking from the 
				// back instead of from the front).
				// I'm correcting this in the indices.
				
				idx[2] = (U16)(y*OverlayDiv+x);
				idx[1] = (U16)((y+1)*OverlayDiv+x);
				idx[0] = (U16)((y+1)*OverlayDiv+x+1);
				idx[5] = (U16)(y*OverlayDiv+x);
				idx[4] = (U16)((y+1)*OverlayDiv+x+1);
				idx[3] = (U16)(y*OverlayDiv+x+1);
			}
		}
		
		vb.reset(); // unmap
	}


	int m_charIdx;
	float m_elapsed;
	float m_time;
	bool m_first;
	pkg::Asset::Ref m_mat;
	asset::MaterialParser::Ref m_parser;
	asset::MaterialLoader::Ref m_loader;
	r::GLVertexBuffer::Ref m_rectVB;
	r::GLVertexBuffer::Ref m_rectIB;
};

Game::Tickable::Ref GSLoadMap::New(int mapId, int slot, bool play, bool loadScreen)
{
	return Game::Tickable::Ref(new (ZWorld) GSCrowLoadMap(mapId, slot, play, loadScreen));
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
