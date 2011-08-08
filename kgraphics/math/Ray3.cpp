#include <stdafx.h>

#include <kgraphics/math/Ray3.h>
#include <kgraphics/math/Line3.h>
#include <kgraphics/math/math.h>
#include <kgraphics/math/Matrix44.h>
#include <kgraphics/math/Quat.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

//----------------------------------------------------------------------------
// @ Ray3::Ray3()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Ray3::Ray3() :
    mOrigin( 0.0f, 0.0f, 0.0f ),
    mDirection( 1.0f, 0.0f, 0.0f )
{
}   // End of Ray3::Ray3()


//----------------------------------------------------------------------------
// @ Ray3::Ray3()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Ray3::Ray3( const Vector3& origin, const Vector3& direction ) :
    mOrigin( origin ),
    mDirection( direction )
{
    mDirection.Normalize();

}   // End of Ray3::Ray3()


//----------------------------------------------------------------------------
// @ Ray3::Ray3()
// ---------------------------------------------------------------------------
// Copy constructor
//-----------------------------------------------------------------------------
Ray3::Ray3( const Ray3& other ) :
    mOrigin( other.mOrigin ),
    mDirection( other.mDirection )
{
    mDirection.Normalize();

}   // End of Ray3::Ray3()


//----------------------------------------------------------------------------
// @ Ray3::operator=()
// ---------------------------------------------------------------------------
// Assigment operator
//-----------------------------------------------------------------------------
Ray3&
Ray3::operator=( const Ray3& other )
{
    // if same object
    if ( this == &other )
        return *this;
        
    mOrigin = other.mOrigin;
    mDirection = other.mDirection;
    mDirection.Normalize();

    return *this;

}   // End of Ray3::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Output a text Ray3.  The format is assumed to be :
// [ Vector3, Vector3 ]
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const Ray3& source)
{
    return out << "[" << source.GetOrigin() 
                     << ", " << source.GetDirection() << "]";
    
}   // End of operator<<()
    

//----------------------------------------------------------------------------
// @ Ray3::Get()
// ---------------------------------------------------------------------------
// Returns the two endpoints
//-----------------------------------------------------------------------------
void
Ray3::Get( Vector3& origin, Vector3& direction ) const
{
    origin = mOrigin;
    direction = mDirection;

}   // End of Ray3::Get()


//----------------------------------------------------------------------------
// @ Ray3::operator==()
// ---------------------------------------------------------------------------
// Are two Ray3's equal?
//----------------------------------------------------------------------------
Bool
Ray3::operator==( const Ray3& ray ) const
{
    return (ray.mOrigin == mOrigin && ray.mDirection == mDirection);

}  // End of Ray3::operator==()


//----------------------------------------------------------------------------
// @ Ray3::operator!=()
// ---------------------------------------------------------------------------
// Are two Ray3's not equal?
//----------------------------------------------------------------------------
Bool
Ray3::operator!=( const Ray3& ray ) const
{
    return !(ray.mOrigin == mOrigin && ray.mDirection == mDirection);
}  // End of Ray3::operator!=()


//----------------------------------------------------------------------------
// @ Ray3::Set()
// ---------------------------------------------------------------------------
// Sets the two parameters
//-----------------------------------------------------------------------------
void
Ray3::Set( const Vector3& origin, const Vector3& direction )
{
    mOrigin = origin;
    mDirection = direction;
    mDirection.Normalize();

}   // End of Ray3::Set()


//----------------------------------------------------------------------------
// @ Ray3::Transform()
// ---------------------------------------------------------------------------
// Transforms ray into new space
//-----------------------------------------------------------------------------
Ray3  
Ray3::Transform( float scale, const Quat& rotate, const Vector3& translate ) const
{
    Ray3 ray;
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

    ray.mDirection = transform.Transform( mDirection );
    ray.mDirection.Normalize();

    transform(0,3) = translate.x;
    transform(1,3) = translate.y;
    transform(2,3) = translate.z;

    ray.mOrigin = transform.Transform( mOrigin );

    return ray;

}   // End of Ray3::Transform()


//----------------------------------------------------------------------------
// @ ::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between rays.
// Based on article and code by Dan Sunday at www.geometryalgorithms.com
//-----------------------------------------------------------------------------
float
DistanceSquared( const Ray3& ray0, const Ray3& ray1, 
                 float& s_c, float& t_c )
{
    // compute intermediate parameters
    Vector3 w0 = ray0.mOrigin - ray1.mOrigin;
    float a = ray0.mDirection.Dot( ray0.mDirection );
    float b = ray0.mDirection.Dot( ray1.mDirection );
    float c = ray1.mDirection.Dot( ray1.mDirection );
    float d = ray0.mDirection.Dot( w0 );
    float e = ray1.mDirection.Dot( w0 );

    float denom = a*c - b*b;
    // parameters to compute s_c, t_c
    float sn, sd, tn, td;

    // if denom is zero, try finding closest point on ray1 to origin0
    if ( gfx::IsZero(denom) )
    {
        // clamp s_c to 0
        sd = td = c;
        sn = 0.0f;
        tn = e;
    }
    else
    {
        // clamp s_c within [0,+inf]
        sd = td = denom;
        sn = b*e - c*d;
        tn = a*e - b*d;
  
        // clamp s_c to 0
        if (sn < 0.0f)
        {
            sn = 0.0f;
            tn = e;
            td = c;
        }
    }

    // clamp t_c within [0,+inf]
    // clamp t_c to 0
    if (tn < 0.0f)
    {
        t_c = 0.0f;
        // clamp s_c to 0
        if ( -d < 0.0f )
        {
            s_c = 0.0f;
        }
        else
        {
            s_c = -d/a;
        }
    }
    else
    {
        t_c = tn/td;
        s_c = sn/sd;
    }

    // compute difference vector and distance squared
    Vector3 wc = w0 + s_c*ray0.mDirection - t_c*ray1.mDirection;
    return wc.Dot(wc);

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ Ray3::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between ray and line.
// Based on article and code by Dan Sunday at www.geometryalgorithms.com
//-----------------------------------------------------------------------------
float
DistanceSquared( const Ray3& ray, const Line3& line, 
                   float& s_c, float& t_c )
{
    // compute intermediate parameters
    Vector3 w0 = ray.mOrigin - line.GetOrigin();
    float a = ray.mDirection.Dot( ray.mDirection );
    float b = ray.mDirection.Dot( line.GetDirection() );
    float c = line.GetDirection().Dot( line.GetDirection() );
    float d = ray.mDirection.Dot( w0 );
    float e = line.GetDirection().Dot( w0 );

    float denom = a*c - b*b;

    // if denom is zero, try finding closest point on ray1 to origin0
    if ( IsZero(denom) )
    {
        s_c = 0.0f;
        t_c = e/c;
        // compute difference vector and distance squared
        Vector3 wc = w0 - t_c*line.GetDirection();
        return wc.Dot(wc);
    }
    else
    {
        // parameters to compute s_c, t_c
        float sn;

        // clamp s_c within [0,1]
        sn = b*e - c*d;
  
        // clamp s_c to 0
        if (sn < 0.0f)
        {
            s_c = 0.0f;
            t_c = e/c;
        }
        // clamp s_c to 1
        else if (sn > denom)
        {
            s_c = 1.0f;
            t_c = (e+b)/c;
        }
        else
        {
            s_c = sn/denom;
            t_c = (a*e - b*d)/denom;
        }

        // compute difference vector and distance squared
        Vector3 wc = w0 + s_c*ray.mDirection - t_c*line.GetDirection();
        return wc.Dot(wc);
    }

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ ::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between ray and point.
//-----------------------------------------------------------------------------
float DistanceSquared( const Ray3& ray, const Vector3& point, 
                       float& t_c ) 
{
    Vector3 w = point - ray.mOrigin;
    float proj = w.Dot(ray.mDirection);
    // origin is closest point
    if ( proj <= 0 )
    {
        t_c = 0.0f;
        return w.Dot(w);
    }
    // else use normal line check
    else
    {
        float vsq = ray.mDirection.Dot(ray.mDirection);
        t_c = proj/vsq;
        return w.Dot(w) - t_c*proj;
    }

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ ClosestPoints()
// ---------------------------------------------------------------------------
// Returns the closest points between two rays
//-----------------------------------------------------------------------------
void ClosestPoints( Vector3& point0, Vector3& point1, 
                    const Ray3& ray0, 
                    const Ray3& ray1 )
{
    // compute intermediate parameters
    Vector3 w0 = ray0.mOrigin - ray1.mOrigin;
    float a = ray0.mDirection.Dot( ray0.mDirection );
    float b = ray0.mDirection.Dot( ray1.mDirection );
    float c = ray1.mDirection.Dot( ray1.mDirection );
    float d = ray0.mDirection.Dot( w0 );
    float e = ray1.mDirection.Dot( w0 );

    float denom = a*c - b*b;
    // parameters to compute s_c, t_c
    float s_c, t_c;
    float sn, sd, tn, td;

    // if denom is zero, try finding closest point on ray1 to origin0
    if ( IsZero(denom) )
    {
        sd = td = c;
        sn = 0.0f;
        tn = e;
    }
    else
    {
        // start by clamping s_c
        sd = td = denom;
        sn = b*e - c*d;
        tn = a*e - b*d;
  
        // clamp s_c to 0
        if (sn < 0.0f)
        {
            sn = 0.0f;
            tn = e;
            td = c;
        }
    }

    // clamp t_c within [0,+inf]
    // clamp t_c to 0
    if (tn < 0.0f)
    {
        t_c = 0.0f;
        // clamp s_c to 0
        if ( -d < 0.0f )
        {
            s_c = 0.0f;
        }
        else
        {
            s_c = -d/a;
        }
    }
    else
    {
        t_c = tn/td;
        s_c = sn/sd;
    }

    // compute closest points
    point0 = ray0.mOrigin + s_c*ray0.mDirection;
    point1 = ray1.mOrigin + t_c*ray1.mDirection;
}


//----------------------------------------------------------------------------
// @ ClosestPoints()
// ---------------------------------------------------------------------------
// Returns the closest points between ray and line
//-----------------------------------------------------------------------------
void ClosestPoints( Vector3& point0, Vector3& point1, 
                    const Ray3& ray, 
                    const Line3& line )
{
    // compute intermediate parameters
    Vector3 w0 = ray.mOrigin - line.GetOrigin();
    float a = ray.mDirection.Dot( ray.mDirection );
    float b = ray.mDirection.Dot( line.GetDirection() );
    float c = line.GetDirection().Dot( line.GetDirection() );
    float d = ray.mDirection.Dot( w0 );
    float e = line.GetDirection().Dot( w0 );

    float denom = a*c - b*b;

    // if denom is zero, try finding closest point on ray1 to origin0
    if ( IsZero(denom) )
    {
        // compute closest points
        point0 = ray.mOrigin;
        point1 = line.GetOrigin() + (e/c)*line.GetDirection();
    }
    else
    {
        // parameters to compute s_c, t_c
        float sn, s_c, t_c;

        // clamp s_c within [0,1]
        sn = b*e - c*d;
  
        // clamp s_c to 0
        if (sn < 0.0f)
        {
            s_c = 0.0f;
            t_c = e/c;
        }
        // clamp s_c to 1
        else if (sn > denom)
        {
            s_c = 1.0f;
            t_c = (e+b)/c;
        }
        else
        {
            s_c = sn/denom;
            t_c = (a*e - b*d)/denom;
        }

        // compute closest points
        point0 = ray.mOrigin + s_c*ray.mDirection;
        point1 = line.GetOrigin() + t_c*line.GetDirection();
    }

}   // End of ClosestPoints()


//----------------------------------------------------------------------------
// @ Ray3::ClosestPoint()
// ---------------------------------------------------------------------------
// Returns the closest point on ray to point
//-----------------------------------------------------------------------------
Vector3 
Ray3::ClosestPoint( const Vector3& point ) const
{
    Vector3 w = point - mOrigin;
    float proj = w.Dot(mDirection);
    // endpoint 0 is closest point
    if ( proj <= 0.0f )
        return mOrigin;
    else
    {
        float vsq = mDirection.Dot(mDirection);
        // else somewhere else in ray
        return mOrigin + (proj/vsq)*mDirection;
    }
}

} // namespace gfx
