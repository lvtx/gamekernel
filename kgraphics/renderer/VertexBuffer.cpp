#include "stdafx.h"

#include <kgraphics/renderer/VertexBuffer.h>

using namespace gfx; 

VertexBuffer::VertexBuffer( IDirect3DVertexBuffer9* vb, 
							uint triangleCount, 
							uint stride, 
							uint fvf )
: m_vb( vb )
, m_triangleCount( triangleCount )
, m_stride( stride )
, m_fvf( fvf )
{
	K_ASSERT( m_vb != 0 );
	K_ASSERT( triangleCount > 0 );
	K_ASSERT( stride > 0 );
	K_ASSERT( fvf > 0 );
}

VertexBuffer::~VertexBuffer()
{
	K_ASSERT( m_vb != 0 );

	m_vb->Release();
	m_vb = 0;
}

bool VertexBuffer::Update( byte* v, uint triangleCount, uint stride )
{
	K_ASSERT( v != 0 );
	K_ASSERT( triangleCount == m_triangleCount );
	K_ASSERT( stride == m_stride );
	K_ASSERT( m_vb != 0 );

	void* pVertices;

	m_vb->Lock( 0, 0, (void**)&pVertices, 0 );

	memcpy( (byte*)pVertices, v, triangleCount*stride);
	
	m_vb->Unlock();

	return true;
}

