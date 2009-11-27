#include "stdafx.h"

#include <kcore/corebase.h>
#include <kcore/sys/PropertySet.h>

namespace gk {

PropertySet::PropertySet()
{
}

PropertySet::~PropertySet()
{
}

PropertySet&
PropertySet::Add( const tstring& name, const Property& v )
{
	if ( Has( name ) )
	{
		props_[name] = v;
	}
	else
	{
		props_.insert( PropertyMap::value_type( name, v ) );
		propNames_.push_back( name );
	}

	return *this;
}

bool 
PropertySet::Has( const tstring& name ) const
{
	return props_.find( name ) != props_.end();
}

const Property& 
PropertySet::Get( const tstring& name ) const
{
	static Property null;

	PropertyMap::const_iterator i( props_.find( name ) );

	if ( i != props_.end() )
	{
		return i->second;
	}

	return null;
}

PropertySet&
PropertySet::Add( const TCHAR* name, const Property& v )
{
	return Add( (tstring)name, v );
}

bool 
PropertySet::Has( const TCHAR* name ) const
{
	return Has( (tstring)name );
}

const Property& 
PropertySet::Get( const TCHAR* name ) const
{
	return Get( (tstring)name );
}

const PropertySet::PropertyNameList& 
PropertySet::GetPropertyNameList() const
{
	return propNames_;
}

void 
PropertySet::Clear()
{
	props_.clear();
}

} // gk
