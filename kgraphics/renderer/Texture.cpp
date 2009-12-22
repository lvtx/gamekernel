#include "stdafx.h"

#include <kgraphics/renderer/Texture.h>

using namespace gfx; 

Texture::Texture( IDirect3DTexture9* tex )
: m_tex( tex )
{
	K_ASSERT( m_tex != 0 );
}

Texture::~Texture()
{
	if ( m_tex != 0 )
	{
		m_tex->Release();
		m_tex = 0;
	}
}
