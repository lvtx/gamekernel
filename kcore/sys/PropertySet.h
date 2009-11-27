#pragma once

#include <kcore/sys/Property.h>

#include <vector>
#include <hash_map>

namespace gk 
{
/**
 * @class PropertySet
 *
 * A class which holds values in name
 */
class PropertySet 
{
public:
	typedef std::vector<tstring> PropertyNameList;

public:
	PropertySet();
	~PropertySet();

	/**
	 * Add a property with name
	 *
	 * @param name The property name
	 * @param v The value 
	 * @return Reference to this object
	 */
	PropertySet& Add( const tstring& name, const Property& v );
	PropertySet& Add( const TCHAR* name, const Property& v );

	/**
	 * Check whether there is a property with the name
	 *
	 * @param name The property name
	 * @return true if there is a property with the name
	 */
	bool Has( const tstring& name ) const;
	bool Has( const TCHAR* name ) const;

	/**
	 * Get a property with the name
	 *
	 * NOTE: Checking with Has must be done.
	 *
	 * @param name The property name
	 * @return Reference to the found property 
	 */
	const Property& Get( const tstring& name ) const;
	const Property& Get( const TCHAR* name ) const;

	/**
	 * Get the list of propery names
	 *
	 * @return PropertyNameList 
	 */
	const PropertySet::PropertyNameList& 
		PropertySet::GetPropertyNameList() const;

	/**
	 * Clear all entries
	 */
	void Clear();

private:
	typedef stdext::hash_map<tstring, Property> PropertyMap;


private:
	PropertyMap props_;
	PropertyNameList propNames_;
};

} // gk 

