#pragma once 

#include <d3d9.h>

namespace gfx {

struct Uv { 
	float u; 
	float v;
};

/**
 * @struct TriMesh 
 * 
 * Default mesh with one tex
 */
struct TriMesh 
{
	static const uint FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1;
	static const uint STRIDE = 36;

	float	x, y, z; 	// vertex pos
	int		color;		// vertex color
	float	nx, ny, nz; // vertex normal
	Uv		tex1; 		// tex coords
};

/**
 * @struct TriSkinMesh 
 *
 * Mesh for skinning with 4 weights
 */
struct TriSkinMesh 
{
	static const uint FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1;
	static const uint STRIDE = 68;

	float x, y, z; 		// vertex pos
	int   color;		// vertex color
	float nx, ny, nz; 	// vertex normal
	Uv	  tex1; 		// tex coords
	float w[4]; 		// weights up to 4 bones
	int   b[4];			// bone indices
};

/**
 * @struct TriBumpSkinMesh
 *
 * Mesh for 4 tex and 4 weights
 */
struct TriBumpSkinMesh 
{
	static const uint FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEX2 | D3DFVF_TEX3;
	static const uint STRIDE = 84;

	float	x, y, z;	// vertex pos
	int		color;		// vertex color
	float	nx, ny, nz; // vertex normal
	Uv		tex[3]; 	// up to 4 textures (diffuse, normal, bump)
	float	w[4]; 		// weights up to 4 bones
	int		b[4];		// bone indices
};

} // namespace gfx
