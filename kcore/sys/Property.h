#pragma once

namespace gk 
{
/**
 * @class Property
 *
 * A simple property class to hold variant values
 */
class Property 
{
public:
	enum Type
	{
		UNKNOWN, 
		BOOL, 
		BYTE, 
		CHAR, 
		SHORT, 
		USHORT, 
		INT, 
		IN64,
		UINT, 
		LONG, 
		ULONG, 
		FLOAT, 
		DOUBLE, 
		VOIDPTR, 
		STRING
	};

public:
	Property();
	~Property();

	Property( const Property& rhs );
	Property& operator=( const Property& rhs );

	Property( bool v );
	Property( byte v );
	Property( TCHAR v );
	Property( short v );
	Property( ushort v );
	Property( int v );
	Property( INT64 v );
	Property( uint v );
	Property( long v );
	Property( ulong v );
	Property( float v );
	Property( double v );
	Property( void* v );
	Property( const TCHAR* v );
	Property( const tstring& v );

	void Set( bool v );
	void Set( byte v );
	void Set( TCHAR v );
	void Set( short v );
	void Set( ushort v );
	void Set( int v );
	void Set( INT64 v );
	void Set( uint v );
	void Set( long v );
	void Set( ulong v );
	void Set( float v );
	void Set( double v );
	void Set( void* v );
	void Set( const TCHAR* v );
	void Set( const tstring& v );

	bool	GetBool() const;
	byte	GetByte() const;
	TCHAR   GetChar() const;
	short	GetShort() const;
	ushort	GetUshort() const;
	int		GetInt() const;
	INT64	GetInt64() const;
	uint	GetUint() const;
	long	GetLong() const;
	ulong	GetUlong() const;
	float	GetFloat() const;
	double	GetDouble() const;
	void*	GetVoidPtr() const;
	const tstring& GetString() const;

	bool IsValid() const;

	Type GetType() const;

	bool operator==( const Property& rhs ) const;
	bool operator!=( const Property& rhs ) const;

private:
	Type type_;

	union Value 
	{
		bool	bo;
		byte	by;
		TCHAR   ch;
		short	sh;
		ushort	ush;
		int		in;
		INT64	in64;
		uint	uin;
		long	lo;
		ulong	ulo;
		float	fl;
		double  dou;
		void*	vo;
	} v_;

	tstring s_;
};

inline
Property::Property()
: type_( UNKNOWN )
{
}

inline
Property::~Property()
{
}

inline
Property::Property( const Property& rhs )
{
	type_ = rhs.type_;

	::memcpy( &v_, &rhs.v_, sizeof( Value ) );
	s_ = rhs.s_;
}

inline
Property& 
Property::operator=( const Property& rhs )
{
	type_ = rhs.type_;

	::memcpy( &v_, &rhs.v_, sizeof( Value ) );
	s_ = rhs.s_;

	return *this;
}

inline
Property::Property( bool v )
{
	Set( v );
}

inline
Property::Property( byte v )
{
	Set( v );
}

inline
Property::Property( TCHAR v )
{
	Set( v );
}

inline
Property::Property( short v )
{
	Set( v );
}

inline
Property::Property( ushort v )
{
	Set( v );
}

inline
Property::Property( int v )
{
	Set( v );
}

inline
Property::Property( INT64 v )
{
	Set( v );
}

inline
Property::Property( uint v )
{
	Set( v );
}

inline
Property::Property( long v )
{
	Set( v );
}

inline
Property::Property( ulong v )
{
	Set( v );
}

inline
Property::Property( float v )
{
	Set( v );
}

inline
Property::Property( double v )
{
	Set( v );
}

inline
Property::Property( void* v )
{
	Set( v );
}

inline
Property::Property( const TCHAR* v )
{
	Set( v );
}

inline
Property::Property( const tstring& v )
{
	Set( v );
}

inline
void 
Property::Set( bool v )
{
	type_ = BOOL;

	v_.bo = v;
}

inline
void 
Property::Set( byte v )
{
	type_ = BYTE;

	v_.by = v;
}

inline
void 
Property::Set( TCHAR v )
{
	type_ = CHAR;

	v_.ch = v;
}

inline
void 
Property::Set( short v )
{
	type_ = SHORT;

	v_.sh = v;
}

inline
void 
Property::Set( ushort v )
{
	type_ = USHORT;

	v_.ush = v;
}

inline
void 
Property::Set( int v )
{
	type_ = INT;

	v_.in = v;
}

inline
void
Property::Set( INT64 v )
{
	type_ = IN64;

	v_.in64 = v;
}

inline
void 
Property::Set( uint v )
{
	type_ = UINT;

	v_.uin = v;
}

inline
void 
Property::Set( long v )
{
	type_ = LONG;

	v_.lo = v;
}

inline
void 
Property::Set( ulong v )
{
	type_ = ULONG;

	v_.ulo = v;
}

inline
void 
Property::Set( float v )
{
	type_ = FLOAT;

	v_.fl = v;
}

inline
void 
Property::Set( double v )
{
	type_ = DOUBLE;

	v_.dou = v;
}

inline
void 
Property::Set( void* v )
{
	type_ = VOIDPTR;

	v_.vo = v;
}

inline
void 
Property::Set( const TCHAR* v )
{
	type_ = STRING;

	s_.clear();
	s_.append( v );
}

inline
void 
Property::Set( const tstring& v )
{
	type_ = STRING;

	s_ = v;
}

inline
bool	
Property::GetBool() const
{
	K_ASSERT( type_ == BOOL );

	return v_.bo;
}

inline
byte	
Property::GetByte() const
{
	K_ASSERT( type_ == BYTE );

	return v_.by;
}

inline
TCHAR
Property::GetChar() const
{
	K_ASSERT( type_ == CHAR );

	return v_.ch;
}

inline
short	
Property::GetShort() const
{
	K_ASSERT( type_ == SHORT );

	return v_.sh;
}

inline
ushort	
Property::GetUshort() const
{
	K_ASSERT( type_ == USHORT );

	return v_.ush;
}

inline
int		
Property::GetInt() const
{
	K_ASSERT( type_ == INT );

	return v_.in;
}

inline
INT64
Property::GetInt64() const
{
	K_ASSERT( type_ == IN64 );

	return v_.in64;
}

inline
uint	
Property::GetUint() const
{
	K_ASSERT( type_ == UINT );

	return v_.uin;
}

inline
long	
Property::GetLong() const
{
	K_ASSERT( type_ == LONG );

	return v_.lo;
}

inline
ulong	
Property::GetUlong() const
{
	K_ASSERT( type_ == ULONG );

	return v_.ulo;
}

inline
float	
Property::GetFloat() const
{
	K_ASSERT( type_ == FLOAT );

	return v_.fl;
}

inline
double	
Property::GetDouble() const
{
	K_ASSERT( type_ == DOUBLE );

	return v_.dou;
}

inline
void*	
Property::GetVoidPtr() const
{
	K_ASSERT( type_ == VOIDPTR );

	return v_.vo;
}

inline
const tstring& 
Property::GetString() const
{
	K_ASSERT( type_ == STRING );

	return s_;
}

inline
bool 
Property::IsValid() const
{
	return ( type_ != UNKNOWN );
}

inline
Property::Type 
Property::GetType() const
{
	return type_;
}


inline
bool 
Property::operator==( const Property& rhs ) const
{
	if ( type_ != rhs.type_ )
	{
		return false;
	}

	if ( type_ == STRING )
	{
		return s_ == rhs.s_;
	}

	return (::memcmp( &v_, &rhs.v_, sizeof( Value ) ) == 0 );
}
inline
bool 
Property::operator!=( const Property& rhs ) const
{
	return !(this->operator==( rhs ));
}

} // gk 
