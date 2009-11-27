#include "stdafx.h"

#include <kcore/corebase.h>
#include <kcore/sys/Buffer.h>

using namespace gk;

Buffer::Buffer( byte *data, uint len )
{
	K_ASSERT( data != 0 );
	K_ASSERT( len > 0 );

	m_data 	= data;
	m_size 	= len;
	m_owner 	= false;
}

Buffer::Buffer( uint len )
{
	K_ASSERT( len > 0 );

	m_size   = len;
	m_data 	 = (byte*)g_allocator.Alloc( m_size );
	m_owner	 = true;
}


