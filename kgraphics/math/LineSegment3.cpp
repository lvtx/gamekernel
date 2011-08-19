#include <stdafx.h>

#include <kgraphics/math/LineSegment3.h>
#include <kgraphics/math/Ray3.h>
#include <kgraphics/math/Line3.h>
#include <kgraphics/math/math.h>
#include <kgraphics/math/Matrix33.h>
#include <kgraphics/math/Matrix44.h>
#include <kgraphics/math/Quat.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

//----------------------------------------------------------------------------
// @ LineSegment3::LineSegment3()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
LineSegment3::LineSegment3() :
    mOrigin( 0.0f, 0.0f, 0.0f ),
    mDirection( 1.0f, 0.0f, 0.0f )
{
}   // End of LineSegment3::LineSegment3()


//----------------------------------------------------------------------------
// @ LineSegment3::LineSegment3()
// ---------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
LineSegment3::LineSegment3( const Vector3& endpoint0, const Vector3& endpoint1 ) :
    mOrigin( endpoint0 ),
    mDirection( endpoint1-endpoint0 )
{
}   // End of LineSegment3::LineSegment3()


//----------------------------------------------------------------------------
// @ LineSegment3::LineSegment3()
// ---------------------------------------------------------------------------
// Copy constructor
//-----------------------------------------------------------------------------
LineSegment3::LineSegment3( const LineSegment3& other ) :
    mOrigin( other.mOrigin ),
    mDirection( other.mDirection )
{
}   // End of LineSegment3::LineSegment3()


//----------------------------------------------------------------------------
// @ LineSegment3::operator=()
// ---------------------------------------------------------------------------
// Assigment operator
//-----------------------------------------------------------------------------
LineSegment3&
LineSegment3::operator=( const LineSegment3& other )
{
    // if same object
    if ( this == &other )
        return *this;
        
    mOrigin = other.mOrigin;
    mDirection = other.mDirection;

    return *this;

}   // End of LineSegment3::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Output a text LineSegment3.  The format is assumed to be :
// [ Vector3, Vector3 ]
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const LineSegment3& source)
{
    return out << "[" << source.GetEndpoint0() 
                     << ", " << source.GetEndpoint1() << "]";
    
}   // End of operator<<()
    

//----------------------------------------------------------------------------
// @ LineSegment3::Get()
// ---------------------------------------------------------------------------
// Returns the two endpoints
//-----------------------------------------------------------------------------
void
LineSegment3::Get( Vector3& endpoint0, Vector3& endpoint1 ) const
{
    endpoint0 = mOrigin;
    endpoint1 = mOrigin + mDirection;

}   // End of LineSegment3::Get()


//----------------------------------------------------------------------------
// @ LineSegment3::Length()
// ---------------------------------------------------------------------------
// Returns the distance between two endpoints
//-----------------------------------------------------------------------------
float
LineSegment3::Length() const
{
    return mDirection.Length();

}   // End of LineSegment3::Length()


//----------------------------------------------------------------------------
// @ LineSegment3::LengthSquared()
// ---------------------------------------------------------------------------
// Returns the squared distance between two endpoints
//-----------------------------------------------------------------------------
float
LineSegment3::LengthSquared() const
{
    return mDirection.LengthSquared();

}   // End of LineSegment3::LengthSquared()


//----------------------------------------------------------------------------
// @ LineSegment3::operator==()
// ---------------------------------------------------------------------------
// Are two LineSegment3's equal?
//----------------------------------------------------------------------------
bool
LineSegment3::operator==( const LineSegment3& segment ) const
{
    return ((segment.mOrigin == mOrigin && segment.mDirection == mDirection) ||
            (segment.mOrigin == mOrigin+mDirection && segment.mDirection == -mDirection));

}  // End of LineSegment3::operator==()


//----------------------------------------------------------------------------
// @ LineSegment3::operator!=()
// ---------------------------------------------------------------------------
// Are two LineSegment3's not equal?
//----------------------------------------------------------------------------
bool
LineSegment3::operator!=( const LineSegment3& segment ) const
{
    return !((segment.mOrigin == mOrigin && segment.mDirection == mDirection) ||
            (segment.mOrigin == mOrigin+mDirection && segment.mDirection == -mDirection));
}  // End of LineSegment3::operator!=()


//----------------------------------------------------------------------------
// @ LineSegment3::Set()
// ---------------------------------------------------------------------------
// Sets the two endpoints
//-----------------------------------------------------------------------------
void
LineSegment3::Set( const Vector3& endpoint0, const Vector3& endpoint1 )
{
    mOrigin = endpoint0;
    mDirection = endpoint1-endpoint0;

}   // End of LineSegment3::Set()


//----------------------------------------------------------------------------
// @ LineSegment3::Transform()
// ---------------------------------------------------------------------------
// Transforms segment into new space
//-----------------------------------------------------------------------------
LineSegment3  
LineSegment3::Transform( float scale, const Quat& rotate, const Vector3& translate ) const
{
    LineSegment3 segment;
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

    segment.mDirection = transform.Transform( mDirection );

    transform(0,3) = translate.x;
    transform(1,3) = translate.y;
    transform(2,3) = translate.z;

    segment.mOrigin = transform.Transform( mOrigin );

    return segment;

}   // End of LineSegment3::Transform()


//----------------------------------------------------------------------------
// @ LineSegment3::Transform()
// ---------------------------------------------------------------------------
// Transforms segment into new space
//-----------------------------------------------------------------------------
LineSegment3  
LineSegment3::Transform( float scale, const Matrix33& rotate, 
                           const Vector3& translate ) const
{
    LineSegment3 segment;
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

    segment.mDirection = transform.Transform( mDirection );

    transform(0,3) = translate.x;
    transform(1,3) = translate.y;
    transform(2,3) = translate.z;

    segment.mOrigin = transform.Transform( mOrigin );

    return segment;

}   // End of LineSegment3::Transform()


//----------------------------------------------------------------------------
// @ ::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between two line segments.
// Based on article and code by Dan Sunday at www.geometryalgorithms.com
//-----------------------------------------------------------------------------
float
DistanceSquared( const LineSegment3& segment0, const LineSegment3& segment1, 
                 float& s_c, float& t_c )
{
    // compute intermediate parameters
    Vector3 w0 = segment0.mOrigin - segment1.mOrigin;
    float a = segment0.mDirection.Dot( segment0.mDirection );
    float b = segment0.mDirection.Dot( segment1.mDirection );
    float c = segment1.mDirection.Dot( segment1.mDirection );
    float d = segment0.mDirection.Dot( w0 );
    float e = segment1.mDirection.Dot( w0 );

    float denom = a*c - b*b;
    // parameters to compute s_c, t_c
    float sn, sd, tn, td;

    // if denom is zero, try finding closest point on segment1 to origin0
    if ( gfx::IsZero(denom) )
    {
        // clamp s_c to 0
        sd = td = c;
        sn = 0.0f;
        tn = e;
    }
    else
    {
        // clamp s_c within [0,1]
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
        // clamp s_c to 1
        else if (sn > sd)
        {
            sn = sd;
            tn = e + b;
            td = c;
        }
    }

    // clamp t_c within [0,1]
    // clamp t_c to 0
    if (tn < 0.0f)
    {
        t_c = 0.0f;
        // clamp s_c to 0
        if ( -d < 0.0f )
        {
            s_c = 0.0f;
        }
        // clamp s_c to 1
        else if ( -d > a )
        {
            s_c = 1.0f;
        }
        else
        {
            s_c = -d/a;
        }
    }
    // clamp t_c to 1
    else if (tn > td)
    {
        t_c = 1.0f;
        // clamp s_c to 0
        if ( (-d+b) < 0.0f )
        {
            s_c = 0.0f;
        }
        // clamp s_c to 1
        else if ( (-d+b) > a )
        {
            s_c = 1.0f;
        }
        else
        {
            s_c = (-d+b)/a;
        }
    }
    else
    {
        t_c = tn/td;
        s_c = sn/sd;
    }

    // compute difference vector and distance squared
    Vector3 wc = w0 + s_c*segment0.mDirection - t_c*segment1.mDirection;
    return wc.Dot(wc);

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ ::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between line segment and ray.
// Based on article and code by Dan Sunday at www.geometryalgorithms.com
//-----------------------------------------------------------------------------
float
DistanceSquared( const LineSegment3& segment, const Ray3& ray,
                 float& s_c, float& t_c )
{
    // compute intermediate parameters
    Vector3 w0 = segment.mOrigin - ray.GetOrigin();
    float a = segment.mDirection.Dot( segment.mDirection );
    float b = segment.mDirection.Dot( ray.GetDirection() );
    float c = ray.GetDirection().Dot( ray.GetDirection() );
    float d = segment.mDirection.Dot( w0 );
    float e = ray.GetDirection().Dot( w0 );

    float denom = a*c - b*b;
    // parameters to compute s_c, t_c
    float sn, sd, tn, td;

    // if denom is zero, try finding closest point on segment1 to origin0
    if ( gfx::IsZero(denom) )
    {
        // clamp s_c to 0
        sd = td = c;
        sn = 0.0f;
        tn = e;
    }
    else
    {
        // clamp s_c within [0,1]
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
        // clamp s_c to 1
        else if (sn > sd)
        {
            sn = sd;
            tn = e + b;
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
        // clamp s_c to 1
        else if ( -d > a )
        {
            s_c = 1.0f;
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
    Vector3 wc = w0 + s_c*segment.mDirection - t_c*ray.GetDirection();
    return wc.Dot(wc);

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ ::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between line segment and line.
// Based on article and code by Dan Sunday at www.geometryalgorithms.com
//-----------------------------------------------------------------------------
float
DistanceSquared( const LineSegment3& segment, const Line3& line,
                 float& s_c, float& t_c )
{
    // compute intermediate parameters
    Vector3 w0 = segment.mOrigin - line.GetOrigin();
    float a = segment.mDirection.Dot( segment.mDirection );
    float b = segment.mDirection.Dot( line.GetDirection() );
    float c = line.GetDirection().Dot( line.GetDirection() );
    float d = segment.mDirection.Dot( w0 );
    float e = line.GetDirection().Dot( w0 );

    float denom = a*c - b*b;

    // if denom is zero, try finding closest point on segment1 to origin0
    if ( gfx::IsZero(denom) )
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
        Vector3 wc = w0 + s_c*segment.mDirection - t_c*line.GetDirection();
        return wc.Dot(wc);
    }

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ LineSegment3::DistanceSquared()
// ---------------------------------------------------------------------------
// Returns the distance squared between line segment and point.
//-----------------------------------------------------------------------------
float 
DistanceSquared( const LineSegment3& segment, const Vector3& point, 
                 float& t_c ) 
{
    Vector3 w = point - segment.mOrigin;
    float proj = w.Dot(segment.mDirection);
    // endpoint 0 is closest point
    if ( proj <= 0 )
    {
        t_c = 0.0f;
        return w.Dot(w);
    }
    else
    {
        float vsq = segment.mDirection.Dot(segment.mDirection);
        // endpoint 1 is closest point
        if ( proj >= vsq )
        {
            t_c = 1.0f;
            return w.Dot(w) - 2.0f*proj + vsq;
        }
        // otherwise somewhere else in segment
        else
        {
            t_c = proj/vsq;
            return w.Dot(w) - t_c*proj;
        }
    }

}   // End of ::DistanceSquared()


//----------------------------------------------------------------------------
// @ ClosestPoints()
// ---------------------------------------------------------------------------
// Returns the closest points between two line segments.
//-----------------------------------------------------------------------------
void ClosestPoints( Vector3& point0, Vector3& point1, 
                    const LineSegment3& segment0, 
                    const LineSegment3& segment1 )
{
    // compute intermediate parameters
    Vector3 w0 = segment0.mOrigin - segment1.mOrigin;
    float a = segment0.mDirection.Dot( segment0.mDirection );
    float b = segment0.mDirection.Dot( segment1.mDirection );
    float c = segment1.mDirection.Dot( segment1.mDirection );
    float d = segment0.mDirection.Dot( w0 );
    float e = segment1.mDirection.Dot( w0 );

    float denom = a*c - b*b;
    // parameters to compute s_c, t_c
    float s_c, t_c;
    float sn, sd, tn, td;

    // if denom is zero, try finding closest point on segment1 to origin0
    if ( gfx::IsZero(denom) )
    {
        // clamp s_c to 0
        sd = td = c;
        sn = 0.0f;
        tn = e;
    }
    else
    {
        // clamp s_c within [0,1]
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
        // clamp s_c to 1
        else if (sn > sd)
        {
            sn = sd;
            tn = e + b;
            td = c;
        }
    }

    // clamp t_c within [0,1]
    // clamp t_c to 0
    if (tn < 0.0f)
    {
        t_c = 0.0f;
        // clamp s_c to 0
        if ( -d < 0.0f )
        {
            s_c = 0.0f;
        }
        // clamp s_c to 1
        else if ( -d > a )
        {
            s_c = 1.0f;
        }
        else
        {
            s_c = -d/a;
        }
    }
    // clamp t_c to 1
    else if (tn > td)
    {
        t_c = 1.0f;
        // clamp s_c to 0
        if ( (-d+b) < 0.0f )
        {
            s_c = 0.0f;
        }
        // clamp s_c to 1
        else if ( (-d+b) > a )
        {
            s_c = 1.0f;
        }
        else
        {
            s_c = (-d+b)/a;
        }
    }
    else
    {
        t_c = tn/td;
        s_c = sn/sd;
    }

    // compute closest points
    point0 = segment0.mOrigin + s_c*segment0.mDirection;
    point1 = segment1.mOrigin + t_c*segment1.mDirection;

}   // End of ClosestPoints()


//----------------------------------------------------------------------------
// @ ClosestPoints()
// ---------------------------------------------------------------------------
// Returns the closest points between line segment and ray.
//-----------------------------------------------------------------------------
void ClosestPoints( Vector3& point0, Vector3& point1, 
                    const LineSegment3& segment, 
                    const Ray3& ray )
{
    // compute intermediate parameters
    Vector3 w0 = segment.mOrigin - ray.GetOrigin();
    float a = segment.mDirection.Dot( segment.mDirection );
    float b = segment.mDirection.Dot( ray.GetDirection() );
    float c = ray.GetDirection().Dot( ray.GetDirection() );
    float d = segment.mDirection.Dot( w0 );
    float e = ray.GetDirection().Dot( w0 );

    float denom = a*c - b*b;
    // parameters to compute s_c, t_c
    float s_c, t_c;
    float sn, sd, tn, td;

    // if denom is zero, try finding closest point on segment1 to origin0
    if ( gfx::IsZero(denom) )
    {
        // clamp s_c to 0
        sd = td = c;
        sn = 0.0f;
        tn = e;
    }
    else
    {
        // clamp s_c within [0,1]
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
        // clamp s_c to 1
        else if (sn > sd)
        {
            sn = sd;
            tn = e + b;
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
        // clamp s_c to 1
        else if ( -d > a )
        {
            s_c = 1.0f;
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
    point0 = segment.mOrigin + s_c*segment.mDirection;
    point1 = ray.GetOrigin() + t_c*ray.GetDirection();

}   // End of ClosestPoints()


//----------------------------------------------------------------------------
// @ LineSegment3::ClosestPoints()
// ---------------------------------------------------------------------------
// Returns the closest points between line segment and line.
//-----------------------------------------------------------------------------
void ClosestPoints( Vector3& point0, Vector3& point1, 
                    const LineSegment3& segment, 
                    const Line3& line )
{
    // compute intermediate parameters
    Vector3 w0 = segment.mOrigin - line.GetOrigin();
    float a = segment.mDirection.Dot( segment.mDirection );
    float b = segment.mDirection.Dot( line.GetDirection() );
    float c = line.GetDirection().Dot( line.GetDirection() );
    float d = segment.mDirection.Dot( w0 );
    float e = line.GetDirection().Dot( w0 );

    float denom = a*c - b*b;

    // if denom is zero, try finding closest point on line to segment origin
    if ( gfx::IsZero(denom) )
    {
        // compute closest points
        point0 = segment.mOrigin;
        point1 = line.GetOrigin() + (e/c)*line.GetDirection();
    }
    else
    {
        // parameters to compute s_c, t_c
        float s_c, t_c;
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

        // compute closest points
        point0 = segment.mOrigin + s_c*segment.mDirection;
        point1 = line.GetOrigin() + t_c*line.GetDirection();
    }

}   // End of ClosestPoints()



//----------------------------------------------------------------------------
// @ LineSegment3::ClosestPoint()
// ---------------------------------------------------------------------------
// Returns the closest point on line segment to point
//-----------------------------------------------------------------------------
Vector3 LineSegment3::ClosestPoint( const Vector3& point ) const
{
    Vector3 w = point - mOrigin;
    float proj = w.Dot(mDirection);
    // endpoint 0 is closest point
    if ( proj <= 0.0f )
        return mOrigin;
    else
    {
        float vsq = mDirection.Dot(mDirection);
        // endpoint 1 is closest point
        if ( proj >= vsq )
            return mOrigin + mDirection;
        // else somewhere else in segment
        else
            return mOrigin + (proj/vsq)*mDirection;
    }
}

} // namespace gfx
