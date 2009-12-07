#pragma once

#include <d3d9.h>

namespace gfx {

/**
 * @class Texture 
 */
class Texture 
{
public:
	Texture( IDirect3DTexture9* tex );
	~Texture();

	IDirect3DTexture9* GetTex();

private:
	IDirect3DTexture9* m_tex;
};

inline
IDirect3DTexture9* 
Texture::GetTex()
{
	return m_tex;
}

} // namespace gfx
