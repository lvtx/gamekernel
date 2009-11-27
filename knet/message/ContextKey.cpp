#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/message/ContextKey.h>

#include <kcore/util/StringUtil.h>

namespace gk { 

ContextKey::ContextKey()
: m_key()
{
}

ContextKey::~ContextKey()
{
}

void 
ContextKey::Update( short v )
{
	Update( (int)v );
}

void 
ContextKey::Update( ushort v )
{
	Update( (int)v );
}

void 
ContextKey::Update( int v )
{
	m_key.append( StringUtil::ToString( v ) );
}

void 
ContextKey::Update( uint v )
{
	Update( (int)v );
}

void 
ContextKey::Update( const tstring& v )
{
	m_key.append( v );
}

void 
ContextKey::Clear()
{
	m_key.clear();
}

const tstring& 
ContextKey::GetKey()
{
	return m_key;
}

} // gk
