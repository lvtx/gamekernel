#include "stdafx.h"

#include "IndexBuffer.h"

#include "Renderer.h"

using namespace gfx; 

IndexBuffer::IndexBuffer( IDirect3DIndexBuffer9* ib, uint faceCount )
: m_ib( ib )
, m_faceCont( faceCount )
{
	K_ASSERT( m_ib != 0 );
	K_ASSERT( m_faceCont > 0 );
}

IndexBuffer::~IndexBuffer()
{
	K_ASSERT( m_ib != 0 );

	m_ib->Release();
	m_ib = 0;
}

bool IndexBuffer::Update( byte* v, uint faceCount )
{
	K_ASSERT( v != 0 );
	K_ASSERT( faceCount <= m_faceCont );
	K_ASSERT( m_ib != 0 );

	void* pindices = 0;

	m_ib->Lock( 0, 0, (void**)pindices, 0 );

	memcpy( (byte*)pindices, v, faceCount * 3 * 2 );  // use short index	

	m_ib->Unlock();
}


