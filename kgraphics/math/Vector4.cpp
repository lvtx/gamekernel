#include "stdafx.H"

#include <kgraphics/math/Vector4.h>
#include <kgraphics/math/math.h>

namespace gfx {

Vector4 Vector4::xAxis( 1.0f, 0.0f, 0.0f, 0.0f );
Vector4 Vector4::yAxis( 0.0f, 1.0f, 0.0f, 0.0f );
Vector4 Vector4::zAxis( 0.0f, 0.0f, 1.0f, 0.0f );
Vector4 Vector4::wAxis( 0.0f, 0.0f, 0.0f, 1.0f );
Vector4 Vector4::origin( 0.0f, 0.0f, 0.0f, 0.0f );

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ Vector4::Vector4()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
Vector4::Vector4(const Vector4& other) : 
    x( other.x ),
    y( other.y ),
    z( other.z ),
    w( other.w )
{

}   // End of Vector4::Vector4()


//-------------------------------------------------------------------------------
// @ Vector4::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
Vector4&
Vector4::operator=(const Vector4& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    
    return *this;

}   // End of Vector4::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const Vector4& source)
{
    out << '<' << source.x << ',' << source.y << ',' << source.z << ','
        << source.w << '>';

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ Vector4::Length()
//-------------------------------------------------------------------------------
// Vector length
//-------------------------------------------------------------------------------
float 
Vector4::Length() const
{
    return Sqrt( x*x + y*y + z*z + w*w );

}   // End of Vector4::Length()


//-------------------------------------------------------------------------------
// @ Vector4::LengthSquared()
//-------------------------------------------------------------------------------
// Vector length squared (avoids square root)
//-------------------------------------------------------------------------------
float 
Vector4::LengthSquared() const
{
    return ( x*x + y*y + z*z + w*w );

}   // End of Vector4::LengthSquared()


//-------------------------------------------------------------------------------
// @ Vector4::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
Vector4::operator==( const Vector4& other ) const
{
    if ( AreEqual( other.x, x )
        && AreEqual( other.y, y )
        && AreEqual( other.z, z )
        && AreEqual( other.w, w ) )
        return true;

    return false;   
}   // End of Vector4::operator==()


//-------------------------------------------------------------------------------
// @ Vector4::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
Vector4::operator!=( const Vector4& other ) const
{
    if ( AreEqual( other.x, x )
        && AreEqual( other.y, y )
        && AreEqual( other.z, z )
        && AreEqual( other.w, w ) )
        return false;

    return true;
}   // End of Vector4::operator!=()


//-------------------------------------------------------------------------------
// @ Vector4::IsZero()
//-------------------------------------------------------------------------------
// Check for zero vector
//-------------------------------------------------------------------------------
bool 
Vector4::IsZero() const
{
    return gfx::IsZero(x*x + y*y + z*z + w*w);

}   // End of Vector4::IsZero()


//-------------------------------------------------------------------------------
// @ Vector4::IsUnit()
//-------------------------------------------------------------------------------
// Check for unit vector
//-------------------------------------------------------------------------------
bool 
Vector4::IsUnit() const
{
    return gfx::IsZero(1.0f - x*x - y*y - z*z - w*w);

}   // End of Vector4::IsUnit()


//-------------------------------------------------------------------------------
// @ Vector4::Clean()
//-------------------------------------------------------------------------------
// Set elements close to zero equal to zero
//-------------------------------------------------------------------------------
void
Vector4::Clean()
{
    if ( gfx::IsZero( x ) )
        x = 0.0f;
    if ( gfx::IsZero( y ) )
        y = 0.0f;
    if ( gfx::IsZero( z ) )
        z = 0.0f;
    if ( gfx::IsZero( w ) )
        w = 0.0f;

}   // End of Vector4::Clean()


//-------------------------------------------------------------------------------
// @ Vector4::Normalize()
//-------------------------------------------------------------------------------
// Set to unit vector
//-------------------------------------------------------------------------------
void
Vector4::Normalize()
{
    float lengthsq = x*x + y*y + z*z + w*w;

	if ( gfx::IsZero( lengthsq ) )
    {
        Zero();
    }
    else
    {
        float factor = InvSqrt( lengthsq );
        x *= factor;
        y *= factor;
        z *= factor;
        w *= factor;
    }

}   // End of Vector4::Normalize()


//-------------------------------------------------------------------------------
// @ Vector4::operator+()
//-------------------------------------------------------------------------------
// Add vector to self and return
//-------------------------------------------------------------------------------
Vector4 
Vector4::operator+( const Vector4& other ) const
{
    return Vector4( x + other.x, y + other.y, z + other.z, w + other.w );

}   // End of Vector4::operator+()


//-------------------------------------------------------------------------------
// @ Vector4::operator+=()
//-------------------------------------------------------------------------------
// Add vector to self, store in self
//-------------------------------------------------------------------------------
Vector4& 
Vector4::operator+=( const Vector4& other )
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;

}   // End of Vector4::operator+=()


//-------------------------------------------------------------------------------
// @ Vector4::operator-()
//-------------------------------------------------------------------------------
// Subtract vector from self and return
//-------------------------------------------------------------------------------
Vector4 
Vector4::operator-( const Vector4& other ) const
{
    return Vector4( x - other.x, y - other.y, z - other.z, w - other.w );

}   // End of Vector4::operator-()


//-------------------------------------------------------------------------------
// @ Vector4::operator-=()
//-------------------------------------------------------------------------------
// Subtract vector from self, store in self
//-------------------------------------------------------------------------------
Vector4& 
Vector4::operator-=( const Vector4& other )
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;

}   // End of Vector4::operator-=()


//-------------------------------------------------------------------------------
// @ operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication
//-------------------------------------------------------------------------------
Vector4   
Vector4::operator*( float scalar )
{
    return Vector4( scalar*x, scalar*y, scalar*z,
                      scalar*w );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication
//-------------------------------------------------------------------------------
Vector4   
operator*( float scalar, const Vector4& vector )
{
    return Vector4( scalar*vector.x, scalar*vector.y, scalar*vector.z,
                      scalar*vector.w );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ Vector4::operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication by self
//-------------------------------------------------------------------------------
Vector4&
Vector4::operator*=( float scalar )
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;

    return *this;

}   // End of Vector4::operator*()


//-------------------------------------------------------------------------------
// @ operator/()
//-------------------------------------------------------------------------------
// Scalar division
//-------------------------------------------------------------------------------
Vector4   
Vector4::operator/( float scalar )
{
    return Vector4( x/scalar, y/scalar, z/scalar, w/scalar );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ Vector4::operator/=()
//-------------------------------------------------------------------------------
// Scalar division by self
//-------------------------------------------------------------------------------
Vector4&
Vector4::operator/=( float scalar )
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;

    return *this;

}   // End of Vector4::operator/=()


//-------------------------------------------------------------------------------
// @ Vector4::Dot()
//-------------------------------------------------------------------------------
// Dot product by self
//-------------------------------------------------------------------------------
float               
Vector4::Dot( const Vector4& vector ) const
{
    return (x*vector.x + y*vector.y + z*vector.z + w*vector.w);

}   // End of Vector4::Dot()


//-------------------------------------------------------------------------------
// @ Dot()
//-------------------------------------------------------------------------------
// Dot product friend operator
//-------------------------------------------------------------------------------
float               
Dot( const Vector4& vector1, const Vector4& vector2 )
{
    return (vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z
            + vector1.w*vector2.w);

}   // End of Dot()

} // namespace gfx
