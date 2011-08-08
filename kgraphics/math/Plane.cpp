#include <stdafx.h>

#include <kgraphics/math/Plane.h>
#include <kgraphics/math/Line3.h>
#include <kgraphics/math/math.h>
#include <kgraphics/math/Matrix33.h>
#include <kgraphics/math/Quat.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

//----------------------------------------------------------------------------
// @ Plane::Plane()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Plane::Plane() :
    mNormal( 1.0f, 0.0f, 0.0f ),
    mOffset( 0.0f )
{
}   // End of Plane::Plane()


//----------------------------------------------------------------------------
// @ Plane::Plane()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Plane::Plane( float a, float b, float c, float d )
{
    Set( a, b, c, d );

}   // End of Plane::Plane()


//----------------------------------------------------------------------------
// @ Plane::Plane()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Plane::Plane( const Vector3& p0, const Vector3& p1, const Vector3& p2 )
{
    Set( p0, p1, p2 );

}   // End of Plane::Plane()


//----------------------------------------------------------------------------
// @ Plane::Plane()
// ---------------------------------------------------------------------------
// Copy constructor
//-----------------------------------------------------------------------------
Plane::Plane( const Plane& other ) :
    mNormal( other.mNormal ),
    mOffset( other.mOffset )
{

}   // End of Plane::Plane()


//----------------------------------------------------------------------------
// @ Plane::operator=()
// ---------------------------------------------------------------------------
// Assigment operator
//-----------------------------------------------------------------------------
Plane&
Plane::operator=( const Plane& other )
{
    // if same object
    if ( this == &other )
        return *this;
        
    mNormal = other.mNormal;
    mOffset = other.mOffset;

    return *this;

}   // End of Plane::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Output a text Plane.  The format is assumed to be :
// [ Vector3, Vector3 ]
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const Plane& source)
{
    return out << "[" << source.mNormal 
                     << ", " << source.mOffset << "]";
    
}   // End of operator<<()
    

//----------------------------------------------------------------------------
// @ Plane::Get()
// ---------------------------------------------------------------------------
// Returns the two endpoints
//-----------------------------------------------------------------------------
void
Plane::Get( Vector3& normal, float& offset ) const
{
    normal = mNormal;
    offset = mOffset;

}   // End of Plane::Get()


//----------------------------------------------------------------------------
// @ Plane::operator==()
// ---------------------------------------------------------------------------
// Are two Plane's equal?
//----------------------------------------------------------------------------
Bool
Plane::operator==( const Plane& plane ) const
{
    return (plane.mNormal == mNormal && plane.mOffset == mOffset);

}  // End of Plane::operator==()


//----------------------------------------------------------------------------
// @ Plane::operator!=()
// ---------------------------------------------------------------------------
// Are two Plane's not equal?
//----------------------------------------------------------------------------
Bool
Plane::operator!=( const Plane& plane ) const
{
    return !(plane.mNormal == mNormal && plane.mOffset == mOffset);
}  // End of Plane::operator!=()


//----------------------------------------------------------------------------
// @ Plane::Set()
// ---------------------------------------------------------------------------
// Sets the parameters
//-----------------------------------------------------------------------------
void
Plane::Set( float a, float b, float c, float d )
{
    // normalize for cheap distance checks
    float lensq = a*a + b*b + c*c;
    // length of normal had better not be zero
    // //ASSERT( !::IsZero( lensq ) );

    // recover gracefully
    if ( gfx::IsZero( lensq ) )
    {
        mNormal = Vector3::xAxis;
        mOffset = 0.0f;
    }
    else
    {
        float recip = InvSqrt(lensq);
        mNormal.Set( a*recip, b*recip, c*recip );
        mOffset = d*recip;
    }

}   // End of Plane::Set()


//----------------------------------------------------------------------------
// @ Plane::Set()
// ---------------------------------------------------------------------------
// Sets the parameters
//-----------------------------------------------------------------------------
void 
Plane::Set( const Vector3& p0, const Vector3& p1, const Vector3& p2 )
{
    // get plane vectors
    Vector3 u = p1 - p0;
    Vector3 v = p2 - p0;
    Vector3 w = u.Cross(v);
    
    // normalize for cheap distance checks
    float lensq = w.x*w.x + w.y*w.y + w.z*w.z;
    // length of normal had better not be zero
    // //ASSERT( !::IsZero( lensq ) );

    // recover gracefully
    if ( gfx::IsZero( lensq ) )
    {
        mNormal = Vector3::xAxis;
        mOffset = 0.0f;
    }
    else
    {
        float recip = 1.0f/lensq;
        mNormal.Set( w.x*recip, w.y*recip, w.z*recip );
        mOffset = -mNormal.Dot(p0);
    }

}   // End of Plane::Set()


//----------------------------------------------------------------------------
// @ Plane::Transform()
// ---------------------------------------------------------------------------
// Transforms plane into new space
//-----------------------------------------------------------------------------
Plane 
Plane::Transform( float scale, const Quat& rotate, const Vector3& translate ) const
{
    Plane plane;

    // get rotation matrix
    Matrix33    rotmatrix(rotate);

    // transform to get normal
    plane.mNormal = rotmatrix*mNormal/scale;
    
    // transform to get offset
    Vector3 newTrans = translate*rotmatrix;
    plane.mOffset = -newTrans.Dot( mNormal )/scale + mOffset;

    return plane;

}   // End of Plane::Transform()


//----------------------------------------------------------------------------
// @ Plane::ClosestPoint()
// ---------------------------------------------------------------------------
// Returns the closest point on plane to point
//-----------------------------------------------------------------------------
Vector3 
Plane::ClosestPoint( const Vector3& point ) const
{    
    return point - Test(point)*mNormal; 

}   // End of Plane::ClosestPoint()


} // namespace gfx
