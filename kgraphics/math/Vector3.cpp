#include <stdafx.h>

#include <kgraphics/math/Vector3.h>
#include <kgraphics/math/math.h>

namespace gfx {

Vector3 Vector3::xAxis( 1.0f, 0.0f, 0.0f );
Vector3 Vector3::yAxis( 0.0f, 1.0f, 0.0f );
Vector3 Vector3::zAxis( 0.0f, 0.0f, 1.0f );
Vector3 Vector3::origin( 0.0f, 0.0f, 0.0f );

//-------------------------------------------------------------------------------
// @ Vector3::Vector3()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
Vector3::Vector3(const Vector3& other) : 
    x( other.x ),
    y( other.y ),
    z( other.z )
{

}   // End of Vector3::Vector3()


//-------------------------------------------------------------------------------
// @ Vector3::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
Vector3&
Vector3::operator=(const Vector3& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    x = other.x;
    y = other.y;
    z = other.z;
    
    return *this;

}   // End of Vector3::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const Vector3& source)
{
    out << '<' << source.x << ',' << source.y << ',' << source.z << '>';

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ Vector3::Length()
//-------------------------------------------------------------------------------
// Vector length
//-------------------------------------------------------------------------------
float 
Vector3::Length() const
{
    return Sqrt( x*x + y*y + z*z );

}   // End of Vector3::Length()


//-------------------------------------------------------------------------------
// @ Vector3::LengthSquared()
//-------------------------------------------------------------------------------
// Vector length squared (avoids square root)
//-------------------------------------------------------------------------------
float 
Vector3::LengthSquared() const
{
    return (x*x + y*y + z*z);

}   // End of Vector3::LengthSquared()


//-------------------------------------------------------------------------------
// @ ::Distance()
//-------------------------------------------------------------------------------
// Point distance
//-------------------------------------------------------------------------------
float 
Distance( const Vector3& p0, const Vector3& p1 )
{
    float x = p0.x - p1.x;
    float y = p0.y - p1.y;
    float z = p0.z - p1.z;

    return Sqrt( x*x + y*y + z*z );

}   // End of Vector3::Length()

//-------------------------------------------------------------------------------
// @ ::Distance2D()
//-------------------------------------------------------------------------------
// Point distance
//-------------------------------------------------------------------------------
float 
Distance2D( const Vector3& p0, const Vector3& p1 )
{
    float x = p0.x - p1.x;
    float y = p0.y - p1.y;
    return Sqrt( x*x + y*y );

}   // End of Vector3::Length()


//-------------------------------------------------------------------------------
// @ ::DistanceSquared()
//-------------------------------------------------------------------------------
// Point distance
//-------------------------------------------------------------------------------
float 
DistanceSquared( const Vector3& p0, const Vector3& p1 )
{
    float x = p0.x - p1.x;
    float y = p0.y - p1.y;
    float z = p0.z - p1.z;

    return ( x*x + y*y + z*z );

}   // End of ::DistanceSquared()


//-------------------------------------------------------------------------------
// @ Vector3::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
Bool 
Vector3::operator==( const Vector3& other ) const
{
    if ( AreEqual( other.x, x )
        && AreEqual( other.y, y )
        && AreEqual( other.z, z ) )
        return true;

    return false;   
}   // End of Vector3::operator==()


//-------------------------------------------------------------------------------
// @ Vector3::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
Bool 
Vector3::operator!=( const Vector3& other ) const
{
    if ( AreEqual( other.x, x )
        && AreEqual( other.y, y )
        && AreEqual( other.z, z ) )
        return false;

    return true;
}   // End of Vector3::operator!=()


//-------------------------------------------------------------------------------
// @ Vector3::IsZero()
//-------------------------------------------------------------------------------
// Check for zero vector
//-------------------------------------------------------------------------------
Bool 
Vector3::IsZero() const
{
    return gfx::IsZero(x*x + y*y + z*z);

}   // End of Vector3::IsZero()


//-------------------------------------------------------------------------------
// @ Vector3::IsUnit()
//-------------------------------------------------------------------------------
// Check for unit vector
//-------------------------------------------------------------------------------
Bool 
Vector3::IsUnit() const
{
    return gfx::IsZero(1.0f - x*x - y*y - z*z);

}   // End of Vector3::IsUnit()


//-------------------------------------------------------------------------------
// @ Vector3::Clean()
//-------------------------------------------------------------------------------
// Set elements close to zero equal to zero
//-------------------------------------------------------------------------------
void
Vector3::Clean()
{
    if ( gfx::IsZero( x ) )
        x = 0.0f;
    if ( gfx::IsZero( y ) )
        y = 0.0f;
    if ( gfx::IsZero( z ) )
        z = 0.0f;

}   // End of Vector3::Clean()


//-------------------------------------------------------------------------------
// @ Vector3::Normalize()
//-------------------------------------------------------------------------------
// Set to unit vector
//-------------------------------------------------------------------------------
void
Vector3::Normalize()
{
    float lengthsq = x*x + y*y + z*z;

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
    }

}   // End of Vector3::Normalize()


//-------------------------------------------------------------------------------
// @ Vector3::operator+()
//-------------------------------------------------------------------------------
// Add vector to self and return
//-------------------------------------------------------------------------------
Vector3 
Vector3::operator+( const Vector3& other ) const
{
    return Vector3( x + other.x, y + other.y, z + other.z );

}   // End of Vector3::operator+()


//-------------------------------------------------------------------------------
// @ Vector3::operator+=()
//-------------------------------------------------------------------------------
// Add vector to self, store in self
//-------------------------------------------------------------------------------
Vector3& 
operator+=( Vector3& self, const Vector3& other )
{
    self.x += other.x;
    self.y += other.y;
    self.z += other.z;

    return self;

}   // End of Vector3::operator+=()


//-------------------------------------------------------------------------------
// @ Vector3::operator-()
//-------------------------------------------------------------------------------
// Subtract vector from self and return
//-------------------------------------------------------------------------------
Vector3 
Vector3::operator-( const Vector3& other ) const
{
    return Vector3( x - other.x, y - other.y, z - other.z );

}   // End of Vector3::operator-()


//-------------------------------------------------------------------------------
// @ Vector3::operator-=()
//-------------------------------------------------------------------------------
// Subtract vector from self, store in self
//-------------------------------------------------------------------------------
Vector3& 
operator-=( Vector3& self, const Vector3& other )
{
    self.x -= other.x;
    self.y -= other.y;
    self.z -= other.z;

    return self;

}   // End of Vector3::operator-=()

//-------------------------------------------------------------------------------
// @ Vector3::operator-=() (unary)
//-------------------------------------------------------------------------------
// Negate self and return
//-------------------------------------------------------------------------------
Vector3
Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}    // End of Vector3::operator-()


//-------------------------------------------------------------------------------
// @ operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication
//-------------------------------------------------------------------------------
Vector3   
Vector3::operator*( float scalar )
{
    return Vector3( scalar*x, scalar*y, scalar*z );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication
//-------------------------------------------------------------------------------
Vector3   
operator*( float scalar, const Vector3& vector )
{
    return Vector3( scalar*vector.x, scalar*vector.y, scalar*vector.z );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ Vector3::operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication by self
//-------------------------------------------------------------------------------
Vector3&
Vector3::operator*=( float scalar )
{
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;

}   // End of Vector3::operator*=()


//-------------------------------------------------------------------------------
// @ operator/()
//-------------------------------------------------------------------------------
// Scalar division
//-------------------------------------------------------------------------------
Vector3   
Vector3::operator/( float scalar )
{
    return Vector3( x/scalar, y/scalar, z/scalar );

}   // End of operator/()


//-------------------------------------------------------------------------------
// @ Vector3::operator/=()
//-------------------------------------------------------------------------------
// Scalar division by self
//-------------------------------------------------------------------------------
Vector3&
Vector3::operator/=( float scalar )
{
    x /= scalar;
    y /= scalar;
    z /= scalar;

    return *this;

}   // End of Vector3::operator/=()


//-------------------------------------------------------------------------------
// @ Vector3::Dot()
//-------------------------------------------------------------------------------
// Dot product by self
//-------------------------------------------------------------------------------
float               
Vector3::Dot( const Vector3& vector ) const
{
    return (x*vector.x + y*vector.y + z*vector.z);

}   // End of Vector3::Dot()


//-------------------------------------------------------------------------------
// @ Dot()
//-------------------------------------------------------------------------------
// Dot product friend operator
//-------------------------------------------------------------------------------
float               
Dot( const Vector3& vector1, const Vector3& vector2 )
{
    return (vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z);

}   // End of Dot()


//-------------------------------------------------------------------------------
// @ Vector3::Cross()
//-------------------------------------------------------------------------------
// Cross product by self
//-------------------------------------------------------------------------------
Vector3   
Vector3::Cross( const Vector3& vector ) const
{
    return Vector3( y*vector.z - z*vector.y,
                      z*vector.x - x*vector.z,
                      x*vector.y - y*vector.x );

}   // End of Vector3::Cross()


//-------------------------------------------------------------------------------
// @ Cross()
//-------------------------------------------------------------------------------
// Cross product friend operator
//-------------------------------------------------------------------------------
Vector3               
Cross( const Vector3& vector1, const Vector3& vector2 )
{
    return Vector3( vector1.y*vector2.z - vector1.z*vector2.y,
                      vector1.z*vector2.x - vector1.x*vector2.z,
                      vector1.x*vector2.y - vector1.y*vector2.x );

}   // End of Cross()


} // namespace gfx
