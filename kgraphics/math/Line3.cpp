#include <stdafx.h>

#include <kgraphics/math/Line3.h>
#include <kgraphics/math/math.h>
#include <kgraphics/math/Matrix44.h>
#include <kgraphics/math/Quat.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

//----------------------------------------------------------------------------
// @ Line3::Line3()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Line3::Line3() :
    mOrigin( 0.0f, 0.0f, 0.0f ),
    mDirection( 1.0f, 0.0f, 0.0f )
{
}   // End of Line3::Line3()


//----------------------------------------------------------------------------
// @ Line3::Line3()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Line3::Line3( const Vector3& origin, const Vector3& direction ) :
    mOrigin( origin ),
    mDirection( direction )
{
    mDirection.Normalize();

}   // End of Line3::Line3()


//----------------------------------------------------------------------------
// @ Line3::Line3()
// ---------------------------------------------------------------------------
// Copy constructor
//-----------------------------------------------------------------------------
Line3::Line3( const Line3& other ) :
    mOrigin( other.mOrigin ),
    mDirection( other.mDirection )
{
    mDirection.Normalize();

}   // End of Line3::Line3()


//----------------------------------------------------------------------------
// @ Line3::operator=()
// ---------------------------------------------------------------------------
// Assigment operator
//-----------------------------------------------------------------------------
Line3&
Line3::operator=( const Line3& other )
{
    // if same object
    if ( this == &other )
        return *this;
        
    mOrigin = other.mOrigin;
    mDirection = other.mDirection;
    mDirection.Normalize();

    return *this;

}   // End of Line3::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Output a text Line3.  The format is assumed to be :
// [ Vector3, Vector3 ]
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const Line3& source)
{
    return out << "[" << source.GetOrigin() 
                     << ", " << source.GetDirection() << "]";
    
}   // End of operator<<()
    

//----------------------------------------------------------------------------
// @ Line3::Get()
// ---------------------------------------------------------------------------
// Returns the two endpoints
//-----------------------------------------------------------------------------
void
Line3::Get( Vector3& origin, Vector3& direction ) const
{
    origin = mOrigin;
    direction = mDirection;

}   // End of Line3::Get()


//----------------------------------------------------------------------------
// @ Line3::operator==()
// ---------------------------------------------------------------------------
// Are two Line3's equal?
//----------------------------------------------------------------------------
bool
Line3::operator==( const Line3& ray ) const
{
    return (ray.mOrigin == mOrigin && ray.mDirection == mDirection);

}  // End of Line3::operator==()


//----------------------------------------------------------------------------
// @ Line3::operator!=()
// ---------------------------------------------------------------------------
// Are two Line3's not equal?
//----------------------------------------------------------------------------
bool
Line3::operator!=( const Line3& ray ) const
{
    return !(ray.mOrigin == mOrigin && ray.mDirection == mDirection);
}  // End of Line3::operator!=()


//----------------------------------------------------------------------------
// @ Line3::Set()
// ---------------------------------------------------------------------------
// Sets the two parameters
//-----------------------------------------------------------------------------
void
Line3::Set( const Vector3& origin, const Vector3& direction )
{
    mOrigin = origin;
    mDirection = direction;
    mDirection.Normalize();

}   // End of Line3::Set()


//----------------------------------------------------------------------------
// @ Line3::Transform()
// ---------------------------------------------------------------------------
// Transforms ray into new space
//-----------------------------------------------------------------------------
Line3 
Line3::Transform( float scale, const Quat& rotate, const Vector3& translate ) const
{
    Line3 line;
    Matrix44    transform(rotate);
    transform(0,0) *= scale;
    transform(1,0) *= scale;
    transform(2,0) *= scale;
    transform(0,1) *= scale;
    transform(1,1) *= scale;
    transform(2,1) *= scale;
    transform(0,2) *= scale;
    transform(1,2) *= scale;
    transform(2,2) *= scale;

    line.mDirection = transform.Transform( mDirection );
    line.mDirection.Normalize();

    transform(0,3) = translate.x;
    transform(1,3) = translate.y;
    transform(2,3) = translate.z;

    line.mOrigin = transform.Transform( mOrigin );

    return line;

}   // End of Line3::Transform()


//----------------------------------------------------------------------------
// @ ::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between lines.
//-----------------------------------------------------------------------------
float DistanceSquared( const Line3& line0, const Line3& line1, 
                       float& s_c, float& t_c )
{
    Vector3 w0 = line0.mOrigin - line1.mOrigin;
    float a = line0.mDirection.Dot( line0.mDirection );
    float b = line0.mDirection.Dot( line1.mDirection );
    float c = line1.mDirection.Dot( line1.mDirection );
    float d = line0.mDirection.Dot( w0 );
    float e = line1.mDirection.Dot( w0 );
    float denom = a*c - b*b;
    if ( gfx::IsZero(denom) )
    {
        s_c = 0.0f;
        t_c = e/c;
        Vector3 wc = w0 - t_c*line1.mDirection;
        return wc.Dot(wc);
    }
    else
    {
        s_c = ((b*e - c*d)/denom);
        t_c = ((a*e - b*d)/denom);
        Vector3 wc = w0 + s_c*line0.mDirection
                          - t_c*line1.mDirection;
        return wc.Dot(wc);
    }

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ ::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between line and point.
//-----------------------------------------------------------------------------
float DistanceSquared( const Line3& line, const Vector3& point, float& t_c )
{
    Vector3 w = point - line.mOrigin;
    float vsq = line.mDirection.Dot(line.mDirection);
    float proj = w.Dot(line.mDirection);
    t_c = proj/vsq; 

    return w.Dot(w) - t_c*proj;

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ ClosestPoints()
// ---------------------------------------------------------------------------
// Returns the closest points between two lines
//-----------------------------------------------------------------------------
void ClosestPoints( Vector3& point0, Vector3& point1, 
                    const Line3& line0, 
                    const Line3& line1 )
{
    // compute intermediate parameters
    Vector3 w0 = line0.mOrigin - line1.mOrigin;
    float a = line0.mDirection.Dot( line0.mDirection );
    float b = line0.mDirection.Dot( line1.mDirection );
    float c = line1.mDirection.Dot( line1.mDirection );
    float d = line0.mDirection.Dot( w0 );
    float e = line1.mDirection.Dot( w0 );

    float denom = a*c - b*b;

    if ( gfx::IsZero(denom) )
    {
        point0 = line0.mOrigin;
        point1 = line1.mOrigin + (e/c)*line1.mDirection;
    }
    else
    {
        point0 = line0.mOrigin + ((b*e - c*d)/denom)*line0.mDirection;
        point1 = line1.mOrigin + ((a*e - b*d)/denom)*line1.mDirection;
    }

}   // End of ClosestPoints()


//----------------------------------------------------------------------------
// @ Line3::ClosestPoint()
// ---------------------------------------------------------------------------
// Returns the closest point on line to point.
//-----------------------------------------------------------------------------
Vector3 Line3::ClosestPoint(const Vector3& point) const
{
    Vector3 w = point - mOrigin;
    float vsq = mDirection.Dot(mDirection);
    float proj = w.Dot(mDirection);

    return mOrigin + (proj/vsq)*mDirection;

}   // End of Line3::ClosestPoint()

} // namespace gfx
