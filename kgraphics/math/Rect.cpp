#include <stdafx.h>

#include <kgraphics/math/Rect.h>
#include <kgraphics/math/math.h>

#include <cassert>

namespace gfx {

Rect::Rect() 
: x0( 0 )
, y0( 0 )
, x1( 0 )
, y1( 0 )
{
}  


Rect::Rect( float x0, float y0, float x1, float y1 )
{
    Set( x0, y0, x1, y1 );
} 

Rect::Rect( const Rect& rhs ) 
{
    if ( this == &rhs )
		return;

	x0 = rhs.x0; 
	y0 = rhs.y0; 
	x1 = rhs.x1; 
	y1 = rhs.y1; 
} 


Rect&
Rect::operator=( const Rect& rhs )
{
    if ( this == &rhs )
        return *this;

	x0 = rhs.x0; 
	y0 = rhs.y0; 
	x1 = rhs.x1; 
	y1 = rhs.y1; 

    return *this;
} 

void 
Rect::Set( float x0, float y0, float x1, float y1 )
{
	assert( x0 < x1 );
	assert( y0 < y1 );

	this->x0 = x0;
	this->y0 = y0;
	this->x1 = x1; 
	this->y1 = y1;
}

void 
Rect::Set( const Vector2& p0, const Vector2& p1 )
{
	Set( p0.x, p0.y, p1.x, p1.y );
}

} // namespace gfx
