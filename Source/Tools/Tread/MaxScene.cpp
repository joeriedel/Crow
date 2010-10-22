// MaxScene.cpp
// Copyright (c) 2010 Pyramind Labs LLC, All Rights Reserved
// Author: Joe Riedel (joeriedel@hotmail.com)
// See Radiance/LICENSE for licensing terms.

#include "stdafx.h"
#include "MaxScene.h"
#include "TreadDoc.h"
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <stdio.h>

typedef unsigned int U32;
typedef unsigned int UReg;
typedef unsigned char U8;

#define RAD_FOURCC(a, b, c, d) ((U32)(((U32)(a)) + (((U32)(b))<<8) + (((U32)(c))<<16)+ (((U32)(d))<<24)))

namespace
{
	enum
	{
		Id = RAD_FOURCC('R', 'S', 'C', 'N'),
		Version = 1,
		MaxUVChannels = 1
	};

	struct Material
	{
		std::string name;
		U32 flags;
		UReg emitId;

		struct Sub
		{
			std::string name;
			UReg id;
			UReg emitId;
		};

		typedef std::map<UReg, Sub> SubHash;
		SubHash subs;
	};

	typedef std::vector<Material> MatVec;
	
	std::string ReadString(FILE *fp)
	{
		std::string s;
		for (;;)
		{
			int c = getc(fp);
			if (!c) break;
			s += (char)c;
		}

		return s;
	}

	vec2 ReadVec2(FILE *fp)
	{
		float v[2];
		fread(v, sizeof(float), 2, fp);
		return vec2(v[0], v[1]);
	}

	vec3 ReadVec3(FILE *fp)
	{
		float v[3];
		fread(v, sizeof(float), 3, fp);
		return vec3(v[0], v[1], v[2]);
	}

	typedef vec2 UV;
	typedef std::vector<UReg> TriFaceIdxVec;
	typedef std::vector<UV> UVVec;

	struct TriVert
	{
		vec3 pos;
		vec3 orgPos;
	};

	typedef std::vector<TriVert> TriVertVec;

	struct UVFace
	{
		int v[3];
	};

	typedef std::vector<UVFace> UVFaceVec;

	struct TriFace
	{
		int v[3];
		int sm[3];
		int smg;
		int mat;
		plane3 plane;
	};

	typedef std::vector<TriFace> TriFaceVec;

	struct TriModel
	{
		UVVec      uvs[MaxUVChannels];
		UVFaceVec  uvtris[MaxUVChannels];
		TriVertVec verts;
		TriFaceVec tris;
		vec3 mins, maxs;
		int contents;
		int id;
	};

	void ReadTriModel(FILE *fp, TriModel &mdl)
	{
		mdl.verts.clear();
		mdl.tris.clear();
		mdl.mins = vec3::bogus_max;
		mdl.maxs = vec3::bogus_min;
		
		for (int i = 0; i < MaxUVChannels; ++i)
		{
			mdl.uvs[i].clear();
			mdl.uvtris[i].clear();
		}

		U32 nv, nf, nc;
		fread(&nv, sizeof(U32), 1, fp);
		fread(&nf, sizeof(U32), 1, fp);
		fread(&nc, sizeof(U32), 1, fp);

		mdl.verts.reserve(nv);
		for (U32 i = 0; i < nv; ++i)
		{
			TriVert v;
			v.orgPos = ReadVec3(fp);
			v.pos[0] = floorf(v.orgPos[0] - 0.5f) + 1.0f;
			v.pos[1] = floorf(v.orgPos[1] - 0.5f) + 1.0f;
			v.pos[2] = floorf(v.orgPos[2] - 0.5f) + 1.0f;
			mdl.mins = vec_mins(mdl.mins, v.pos);
			mdl.maxs = vec_maxs(mdl.maxs, v.pos);
			mdl.verts.push_back(v);
		}

		for (U32 i = 0; i < nc; ++i)
		{
			U32 nuv;
			fread(&nuv, sizeof(U32), 1, fp);

			if (i < MaxUVChannels)
			{
				mdl.uvs[i].reserve(nuv);
				mdl.uvtris[i].reserve(nf);
			}

			for (U32 j = 0; j < nuv; ++j)
			{
				vec2 v = ReadVec2(fp); // ignore
				
				if (i < MaxUVChannels)
				{
					mdl.uvs[i].push_back(v);
				}
			}
			for (U32 j = 0; j < nf; ++j)
			{
				UVFace f;
				fread(&f.v[0], sizeof(int), 1, fp);
				fread(&f.v[1], sizeof(int), 1, fp);
				fread(&f.v[2], sizeof(int), 1, fp);
								
				if (i < MaxUVChannels)
				{
					mdl.uvtris[i].push_back(f);
				}
			}
		}
		
		mdl.tris.reserve(nf);
		bool warn = false;
		for (U32 i = 0; i < nf; ++i)
		{
			TriFace f;
			fread(&f.v[0], sizeof(int), 1, fp);
			fread(&f.v[1], sizeof(int), 1, fp);
			fread(&f.v[2], sizeof(int), 1, fp);
			fread(&f.smg, sizeof(int), 1, fp);
			fread(&f.mat, sizeof(int), 1, fp);

			int z;
			for (z = 0; z < 2; ++z)
			{
				if (mdl.verts[f.v[z]].pos == mdl.verts[f.v[z+1]].pos) break;
				if (mdl.verts[f.v[z]].pos == mdl.verts[f.v[(z+2)%3]].pos) break;
			}
			if (z == 2)
			{
				f.plane = plane3(mdl.verts[f.v[0]].pos, mdl.verts[f.v[1]].pos, mdl.verts[f.v[2]].pos);
				mdl.tris.push_back(f);
			}
			else if (!warn)
			{
				warn = true;
				Sys_printf("WARNING: mesh id %d has degenerate triangles.\n", mdl.id);
			}
		}
	}

	struct SmoothVert
	{
		vec3 pos; // rounded vertex position.
		vec3 orgPos; // original vertex position.
		vec2 st[MaxUVChannels];
		vec3 normal;
		vec3 color;
		TriFaceIdxVec faces;
		int sm;

		bool operator < (const SmoothVert &v)
		{
			for (int i = 0; i < 3; ++i)
			{
				if (pos[i] < v.pos[i]) return true;
			}
			for (int i = 0; i < MaxUVChannels; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					if (st[i][j] < v.st[i][j]) return true;
				}
			}
			return sm < v.sm;
		}
	};

	typedef std::vector<SmoothVert> SmoothVertVec;

	void AddFaces(const SmoothVert &src, SmoothVert &dst)
	{
		dst.faces.reserve(dst.faces.size()+src.faces.size());
		TriFaceIdxVec faces;
		for (TriFaceIdxVec::const_iterator it = src.faces.begin(); it != src.faces.end(); ++it)
		{
			TriFaceIdxVec::iterator it2;
			for (it2 = dst.faces.begin(); it2 != dst.faces.end(); ++it2)
			{
				if (*it == *it2) break;
			}
			if (it2 != dst.faces.end()) continue;
			faces.push_back(*it);
		}

		std::copy(faces.begin(), faces.end(), std::back_insert_iterator<TriFaceIdxVec>(dst.faces));
	}

	void CombineFaces(SmoothVert &a, SmoothVert &b)
	{
		a.faces.reserve(a.faces.size()+b.faces.size());
		b.faces.reserve(a.faces.size()+b.faces.size());
		TriFaceIdxVec faces = a.faces;
		for (TriFaceIdxVec::iterator it = b.faces.begin(); it != b.faces.end(); ++it)
		{
			TriFaceIdxVec::iterator it2;
			for (it2 = faces.begin(); it2 != faces.end(); ++it2)
			{
				if (*it == *it2) 
					break;
			}
			if (it2 != faces.end()) 
				continue;
			faces.push_back(*it);
		}

		a.faces = faces;
		b.faces = faces;
	}

	int HashVert(const SmoothVert &v, SmoothVertVec &vec)
	{
		SmoothVertVec::iterator it;
		for (it = vec.begin(); it != vec.end(); ++it)
		{
			SmoothVert &x = *it;
			if (x.pos != v.pos) 
				continue;
			int i;
			for (i = 0; i < MaxUVChannels; ++i)
			{
				if (x.st[i] != v.st[i]) 
					break;
			}
			if (i != MaxUVChannels) 
				continue;

			if (v.sm & x.sm) // smooth across this vertex?
			{
				x.sm |= v.sm;
				AddFaces(v, x);
				return (int)(it-vec.begin());
			}
		}

		vec.push_back(v);
		return (int)(vec.size()-1);
	}

	void MakeNormals(const TriModel &mdl, SmoothVertVec &vec)
	{
		// some coincedent vertices may not have been combined
		// because their UV's are different. smooth over these as well.
		for (SmoothVertVec::iterator it = vec.begin(); it != vec.end(); ++it)
		{
			SmoothVert &a = *it;
			for (SmoothVertVec::iterator it2 = vec.begin(); it2 != vec.end(); ++it2)
			{
				if (it == it2) continue;
				SmoothVert &b = *it2;

				if (a.pos != b.pos) continue;
				if (a.sm & b.sm) // smooth?
				{
					a.sm = b.sm = (a.sm|b.sm);
					CombineFaces(a, b);
				}
			}
		}

		for (SmoothVertVec::iterator it = vec.begin(); it != vec.end(); ++it)
		{
			SmoothVert &v = *it;
			v.normal = vec3::zero;
			
			for (TriFaceIdxVec::iterator it2 = v.faces.begin(); it2 != v.faces.end(); ++it2)
			{
				U32 idx = *it2;
				v.normal += mdl.tris[idx].plane;
				v.normal.normalize();
			}
		}
	}

	// apply smoothing groups and make render meshes (split by material)
	void Build(
		const std::vector<std::string> &matnames,
		TriModel &mdl,
		std::vector<CRenderMesh::Ref> &out, 
		vec3 &outMins, 
		vec3 &outMaxs
	)
	{
		SmoothVertVec smv;
		SmoothVert v;
		const UVVec     *uvv = mdl.uvs;
		const UVFaceVec *uvf = mdl.uvtris;

		smv.reserve(mdl.verts.size());

		struct MatGroup
		{
			int id;
			std::vector<int> tris;
			SmoothVertVec verts;
			std::map<int, int> vmap;
		};

		typedef std::map<int, MatGroup> MatMap;
		MatMap matMap;

		outMins = vec_mins(mdl.mins, outMins);
		outMaxs = vec_maxs(mdl.maxs, outMaxs);

		UReg idx = 0;
		for (TriFaceVec::iterator it = mdl.tris.begin(); it != mdl.tris.end(); ++it, ++idx)
		{
			TriFace &tri = *it;

			for (int i = 0; i < 3; ++i)
			{
				v.faces.clear();
				v.faces.push_back(idx);
				v.pos = mdl.verts[tri.v[i]].pos;
				v.orgPos = mdl.verts[tri.v[i]].orgPos;
				v.sm  = tri.smg;

				for (int j = 0; j < MaxUVChannels; ++j)
				{
					if (uvv[j].empty())
					{
						v.st[j] = UV::zero;
					}
					else
					{
						v.st[j] = uvv[j][uvf[j][idx].v[i]];
					}
				}

				// reverse winding
				tri.sm[2-i] = HashVert(v, smv);
			}

			MatGroup &group = matMap[tri.mat];
			group.id = tri.mat;
		}

		MakeNormals(mdl, smv);

		for (TriFaceVec::iterator it = mdl.tris.begin(); it != mdl.tris.end(); ++it)
		{
			TriFace &tri = *it;

			MatGroup &group = matMap[tri.mat];

			for (int i = 0; i < 3; ++i)
			{
				std::map<int, int>::iterator it = group.vmap.find(tri.sm[i]);
				if (it == group.vmap.end())
				{
					it = group.vmap.insert(std::map<int,int>::value_type(tri.sm[i], (int)group.verts.size())).first;
					group.verts.push_back(smv[tri.sm[i]]);
				}

				// index cmds for tris.
				group.tris.push_back(it->second);
			}
		}

		for (MatMap::const_iterator it = matMap.begin(); it != matMap.end(); ++it)
		{
			const MatGroup &group = it->second;
			CRenderMesh::Ref mesh(new CRenderMesh());
			mesh->num_pts = (int)group.verts.size();
			mesh->xyz = new vec3[mesh->num_pts];
			mesh->normals[0] = new vec3[mesh->num_pts];
			mesh->st = new vec2[mesh->num_pts];
			mesh->color2d = 0xFF4080FF;
			mesh->color3d = 0xFFFFFFFF;
			mesh->wireframe3d = 0xFF4080FF;
			mesh->solid2d = false;

			if (group.id != -1)
				mesh->shader = Sys_GetActiveDocument()->ShaderForName(matnames[group.id].c_str());
			
			for (unsigned int i = 0; i < group.verts.size(); ++i)
			{
				mesh->xyz[i] = group.verts[i].orgPos;
				mesh->normals[0][i] = -group.verts[i].normal;
				mesh->st[i] = group.verts[i].st[0];
			}

			mesh->num_tris = (int)group.tris.size() / 3;
			mesh->tris = new unsigned int[group.tris.size()];
			memcpy(mesh->tris, &group.tris[0], sizeof(unsigned int)*group.tris.size());

			out.push_back(mesh);
		}
	}
}

bool MaxScene::Load(const char *filename)
{
	this->meshes.clear();
	this->mins = vec3::bogus_max;
	this->maxs = vec3::bogus_min;
	this->origin = vec3::zero;

	FILE *fp = fopen(filename, "rb");
	if (!fp)
		return false;
	
	U32 id, version;

	fread(&id, sizeof(U32), 1, fp);
	fread(&version, sizeof(U32), 1, fp);

	if (id != Id || version != Version)
	{
		fclose(fp);
		return false;
	}

	MatVec mats;
	U32 n;

	// materials.
	std::vector<std::string> matnames;

	fread(&n, sizeof(U32), 1, fp);

	for (U32 i = 0; i < n; ++i)
	{
		Material m;
		m.name = ReadString(fp);
		fread(&m.flags, sizeof(U32), 1, fp);
		
		if (m.flags & 1) // multisub
		{
			U32 z;
			fread(&z, sizeof(U32), 1, fp);
			for (U32 j = 0; j < z; ++j)
			{
				Material::Sub s;
				fread(&s.id, sizeof(U32), 1, fp);
				s.name = ReadString(fp);
				s.emitId = (UReg)matnames.size();
				matnames.push_back(s.name);
				m.subs.insert(Material::SubHash::value_type(s.id, s));
			}
		}
		else
		{
			m.emitId = (UReg)matnames.size();
			matnames.push_back(m.name);
		}
		
		mats.push_back(m);
	}

	// entities

	fread(&n, sizeof(U32), 1, fp);

	TriModel mdl;
	
	for (U32 i = 0; i < n; ++i)
	{
		ReadString(fp);
		U32 unused;
		fread(&unused, sizeof(U32), 1, fp);
		origin += ReadVec3(fp);

		U32 z;
		fread(&z, sizeof(U32), 1, fp);

		for (U32 j = 0; j < z; ++j)
		{
			Material *m = 0;
			U32 flags;

			fread(&mdl.id, sizeof(int), 1, fp);
			fread(&flags, sizeof(U32), 1, fp);

			if (flags & 0x80000000) // has material
			{
				U32 idx;
				fread(&idx, sizeof(U32), 1, fp);
				m = &mats[idx];
			}

			/*if (flags & 0x40000000)
			{
				mdl.contents = Map::ContentsDetail;
			}
			else if (flags & 0x20000000)
			{
				mdl.contents = Map::ContentsAreaportal;
			}
			else
			{
				mdl.contents = Map::ContentsSolid;
			}

			if (!(flags & 0x00800000))
			{
				mdl.contents |= Map::ContentsNoClip;
			}

			if (!(flags & 0x00400000))
			{
				mdl.contents |= Map::ContentsNoDraw;
			}*/

			ReadTriModel(fp, mdl);

			if (mdl.tris.empty())
			{
				Sys_printf("WARNING: model id %d has no valid triangles.\n", mdl.id);
			}
			else
			{
				for (TriFaceVec::iterator it = mdl.tris.begin(); it != mdl.tris.end(); ++it)
				{
					if (m)
					{
						Material::SubHash::iterator sub = m->subs.find((U32)it->mat);
						if (sub != m->subs.end())
						{
							it->mat = (int)sub->second.emitId;
						}
						else
						{
							it->mat = m->emitId;
						}
					}
					else
					{
						it->mat = -1;
					}
				}

				Build(matnames, mdl, this->meshes, this->mins, this->maxs);
			}
		}
	}

	this->origin /= (float)n;

	fclose(fp);

	return true;
}

void MaxScene::Translate(vec3 t)
{
//	mins += t;
//	maxs += t;
	origin += t;

	for (std::vector<CRenderMesh::Ref>::iterator it = meshes.begin(); it != meshes.end(); ++it)
	{
		const CRenderMesh::Ref &mesh = *it;
		for (unsigned int i = 0; i < mesh->num_pts; ++i)
			mesh->xyz[i] += t;
	}
}
