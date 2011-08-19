#include <stdafx.h>

#include <kgraphics/math/Capsule.h>
#include <kgraphics/math/Covariance.h>
#include <kgraphics/math/Line3.h>
#include <kgraphics/math/math.h>
#include <kgraphics/math/Matrix33.h>
#include <kgraphics/math/Plane.h>
#include <kgraphics/math/Vector3.h>
#include <float.h>

namespace gfx { 

//-------------------------------------------------------------------------------
// @ Capsule::Capsule()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
Capsule::Capsule(const Capsule& other) : 
    mSegment( other.mSegment ),
    mRadius( other.mRadius )
{

}   // End of Capsule::Capsule()


//-------------------------------------------------------------------------------
// @ Capsule::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
Capsule&
Capsule::operator=(const Capsule& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    mSegment = other.mSegment;
    mRadius = other.mRadius;
    
    return *this;

}   // End of Capsule::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const Capsule& source)
{
    out << source.mSegment;
    out << ' ' << source.mRadius;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ Capsule::Length()
//-------------------------------------------------------------------------------
// Vector length
//-------------------------------------------------------------------------------
float 
Capsule::Length() const
{
    return mSegment.Length() + 2.0f*mRadius;

}   // End of Capsule::Length()


//-------------------------------------------------------------------------------
// @ Capsule::LengthSquared()
//-------------------------------------------------------------------------------
// Capsule length squared (avoids square root)
//-------------------------------------------------------------------------------
float 
Capsule::LengthSquared() const
{
    return mSegment.LengthSquared();

}   // End of Capsule::LengthSquared()


//-------------------------------------------------------------------------------
// @ Capsule::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
Capsule::operator==( const Capsule& other ) const
{
    if ( other.mSegment == mSegment && other.mRadius == mRadius )
        return true;

    return false;   
}   // End of Capsule::operator==()


//-------------------------------------------------------------------------------
// @ Capsule::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
Capsule::operator!=( const Capsule& other ) const
{
    if ( other.mSegment == mSegment && other.mRadius == mRadius )
        return false;

    return true;
}   // End of Capsule::operator!=()


//-------------------------------------------------------------------------------
// @ Capsule::Set()
//-------------------------------------------------------------------------------
// Set capsule based on set of points
//-------------------------------------------------------------------------------
void
Capsule::Set( const Vector3* points, unsigned int nPoints )
{
    // //ASSERT( points );

    Vector3 centroid;

    // compute covariance matrix
    Matrix33 C;
    ComputeCovarianceMatrix( C, centroid, points, nPoints );

    // get main axis
    Vector3 w, u, v;
    GetRealSymmetricEigenvectors( w, u, v, C );

    float maxDistSq = 0.0f;

    // for each point do
    unsigned int i;
    for ( i = 0; i < nPoints; ++i )
    {
        // compute radius
        Vector3 diff = points[i]-centroid;
        float proj = diff.Dot(w);

        float distSq = diff.Dot(diff) - proj*proj;
        if (distSq > maxDistSq)
          maxDistSq = distSq;
    }

    mRadius = Sqrt(maxDistSq);

    // now set endcaps
    // for each point do
    float t0 = FLT_MAX;
    float t1 = FLT_MIN;
    for ( i = 0; i < nPoints; ++i )
    {
        Vector3 localTrans = points[i]-centroid;
        float uCoord = localTrans.Dot(u);
        float vCoord = localTrans.Dot(v);
        float wCoord = localTrans.Dot(w);

        float radical = maxDistSq - uCoord*uCoord - vCoord*vCoord;
        if ( radical > kEpsilon )
            radical = Sqrt( radical );
        else
            radical = 0.0f;

        float test = wCoord + radical;
        if ( test < t0 )
            t0 = test;

        test = wCoord - radical;
        if ( test > t1 )
            t1 = test;
    }

    // set up segment
    if ( t0 < t1 )
    {
        mSegment.Set( centroid + t0*w, centroid + t1*w );
    }
    else
    {
        // is sphere
        Vector3 center = centroid + 0.5f*(t0+t1)*w;
        mSegment.Set( center, center );
    }

}   // End of Capsule::Set()


//----------------------------------------------------------------------------
// @ Capsule::Transform()
// ---------------------------------------------------------------------------
// Transforms segment into new space
//-----------------------------------------------------------------------------
Capsule   
Capsule::Transform( float scale, const Quat& rotate, 
    const Vector3& translate ) const
{
    return Capsule( mSegment.Transform( scale, rotate, translate ), mRadius*scale );

}   // End of Capsule::Transform()


//----------------------------------------------------------------------------
// @ Capsule::Transform()
// ---------------------------------------------------------------------------
// Transforms segment into new space
//-----------------------------------------------------------------------------
Capsule   
Capsule::Transform( float scale, const Matrix33& rotate, 
    const Vector3& translate ) const
{
    return Capsule( mSegment.Transform( scale, rotate, translate ), 
        mRadius*scale );

}   // End of Capsule::Transform()


//----------------------------------------------------------------------------
// @ Capsule::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between capsule and capsule
//-----------------------------------------------------------------------------
bool 
Capsule::Intersect( const Capsule& other ) const
{
    float radiusSum = mRadius + other.mRadius;

    // if colliding
    float s, t;
    float distancesq = gfx::DistanceSquared( mSegment, other.mSegment, s, t ); 

    return ( distancesq <= radiusSum*radiusSum );
}


//----------------------------------------------------------------------------
// @ Capsule::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between capsule and line
//-----------------------------------------------------------------------------
bool
Capsule::Intersect( const Line3& line ) const
{
    // test distance between line and segment vs. radius
    float s_c, t_c;
    return ( gfx::DistanceSquared( mSegment, line, s_c, t_c ) <= mRadius*mRadius );
}


//----------------------------------------------------------------------------
// @ Capsule::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between capsule and ray
//-----------------------------------------------------------------------------
bool
Capsule::Intersect( const Ray3& ray ) const
{
    // test distance between ray and segment vs. radius
    float s_c, t_c;
    return ( gfx::DistanceSquared( mSegment, ray, s_c, t_c ) <= mRadius*mRadius );

}


//----------------------------------------------------------------------------
// @ Capsule::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between capsule and line segment
//-----------------------------------------------------------------------------
bool
Capsule::Intersect( const LineSegment3& segment ) const
{
    // test distance between segment and segment vs. radius
    float s_c, t_c;
    return ( gfx::DistanceSquared( mSegment, segment, s_c, t_c ) <= mRadius*mRadius );

}


//----------------------------------------------------------------------------
// @ Capsule::Classify()
// ---------------------------------------------------------------------------
// Return signed distance between capsule and plane
//-----------------------------------------------------------------------------
float
Capsule::Classify( const Plane& plane ) const
{
    float s0 = plane.Test( mSegment.GetEndpoint0() );
    float s1 = plane.Test( mSegment.GetEndpoint1() );

    // points on opposite sides or intersecting plane
    if ( s0*s1 <= 0.0f )
        return 0.0f;

    // intersect if either endpoint is within radius distance of plane
    if( Abs(s0) <= mRadius || Abs(s1) <= mRadius )
        return 0.0f;

    // return signed distance
    return ( Abs(s0) < Abs(s1) ? s0 : s1 );
}


//----------------------------------------------------------------------------
// @ ::Merge()
// ---------------------------------------------------------------------------
// Merge two capsules together to create a new one
// See _3D Game Engine Design_ (Eberly) for more details
//-----------------------------------------------------------------------------
void
Merge( Capsule& result, const Capsule& b0, const Capsule& b1 )
{
    // 1) get line through center

    // origin is average of centers
    Vector3 origin = 0.5f*(b0.mSegment.GetCenter() + b1.mSegment.GetCenter());
    
    // direction is average of directions
    Vector3 direction0 = b0.mSegment.GetDirection();
    direction0.Normalize();
    Vector3 direction1 = b1.mSegment.GetDirection();
    direction1.Normalize();
    if (direction0.Dot(direction1) < 0.0f)
        direction1 = -direction1;
    Vector3 direction = direction0 + direction1;

    // create line
    Line3 line( origin, direction );

    // 2) compute radius

    // radius will be the maximum of the distance to each endpoint, 
    // plus the corresponding capsule radius
    float t;
    float radius = Distance( line, b0.mSegment.GetEndpoint0(), t ) + b0.mRadius;
    float tempRadius = Distance( line, b0.mSegment.GetEndpoint1(), t ) + b0.mRadius;
    if ( tempRadius > radius )
        radius = tempRadius;
    tempRadius = Distance( line, b1.mSegment.GetEndpoint0(), t ) + b1.mRadius;
    if ( tempRadius > radius )
        radius = tempRadius;
    tempRadius = Distance( line, b1.mSegment.GetEndpoint1(), t ) + b1.mRadius;
    if ( tempRadius > radius )
        radius = tempRadius;

    // 3) compute parameters of endpoints on line

    // we compute a sphere at each original endpoint, and set the endcaps of the
    // new capsule around them, minimizing size

    // capsule 0, endpoint 0
    float radiusDiffSq = radius - b0.GetRadius();
    radiusDiffSq *= radiusDiffSq;
    Vector3 originDiff = line.GetOrigin() - b0.mSegment.GetEndpoint0();
    // compute coefficients for quadratic
    float halfb = line.GetDirection().Dot( originDiff );
    float c = originDiff.Dot(originDiff) - radiusDiffSq;
    // this is sqrt( b^2 - 4ac )/2, but a lot of terms have cancelled out
    float radical = halfb*halfb - c;
    if ( radical > kEpsilon )
        radical = Sqrt( radical );
    else
        radical = 0.0f;
    // and this is adding -b/2 +/- the above
    float t0 = -halfb - radical;
    float t1 = -halfb + radical;

    // capsule 0, endpoint 1
    originDiff = line.GetOrigin() - b0.mSegment.GetEndpoint1();
    halfb = line.GetDirection().Dot( originDiff );
    c = originDiff.Dot(originDiff) - radiusDiffSq;
    radical = halfb*halfb - c;
    if ( radical > kEpsilon )
        radical = Sqrt( radical );
    else
        radical = 0.0f;
    float param = -halfb - radical;
    if ( param < t0 )
        t0 = param;
    param = -halfb + radical;
    if ( param > t1 )
        t1 = param;

    // capsule 1, endpoint 0
    radiusDiffSq = radius - b1.GetRadius();
    radiusDiffSq *= radiusDiffSq;
    originDiff = line.GetOrigin() - b1.mSegment.GetEndpoint0();
    halfb = line.GetDirection().Dot( originDiff );
    c = originDiff.Dot(originDiff) - radiusDiffSq;
    radical = halfb*halfb - c;
    if ( radical > kEpsilon )
        radical = Sqrt( radical );
    else
        radical = 0.0f;
    param = -halfb - radical;
    if ( param < t0 )
        t0 = param;
    param = -halfb + radical;
    if ( param > t1 )
        t1 = param;

     // capsule 1, endpoint 1
    originDiff = line.GetOrigin() - b1.mSegment.GetEndpoint1();
    halfb = line.GetDirection().Dot( originDiff );
    c = originDiff.Dot(originDiff) - radiusDiffSq;
    radical = halfb*halfb - c;
    if ( radical > kEpsilon )
        radical = Sqrt( radical );
    else
        radical = 0.0f;
    param = -halfb - radical;
    if ( param < t0 )
        t0 = param;
    param = -halfb + radical;
    if ( param > t1 )
        t1 = param;

    // set capsule
    result.mRadius = radius;
    if ( t0 < t1 )
    {
        result.mSegment.Set( line.GetOrigin() + t0*line.GetDirection(), 
                      line.GetOrigin() + t1*line.GetDirection() );
    }
    else
    {
        // is sphere
        Vector3 center = line.GetOrigin() + 0.5f*(t0+t1)*line.GetDirection();
        result.mSegment.Set( center, center );
    }

}   // End of ::Merge()



//----------------------------------------------------------------------------
// @ Capsule::ComputeCollision()
// ---------------------------------------------------------------------------
// Compute parameters for collision between capsule and capsule
//-----------------------------------------------------------------------------
bool 
Capsule::ComputeCollision( const Capsule& other, Vector3& collisionNormal, 
                      Vector3& collisionPoint, float& penetration ) const
{
    float radiusSum = mRadius + other.mRadius;

    // if colliding
    float s, t;
    float distancesq = gfx::DistanceSquared( mSegment, other.mSegment, s, t ); 

    if ( distancesq <= radiusSum*radiusSum )
    {
        // calculate our values
        Vector3 nearPoint0 = (1.0f-s)*mSegment.GetEndpoint0() + s*mSegment.GetEndpoint1();
        Vector3 nearPoint1 = (1.0f-t)*other.mSegment.GetEndpoint0() + t*other.mSegment.GetEndpoint1();

        collisionNormal = nearPoint1 - nearPoint0;  

        // penetration is distance - radii
        float distance = Sqrt(distancesq);
        penetration = radiusSum - distance;
        collisionNormal.Normalize();

        // collision point is average of penetration
        // weighted towards lighter object
        float t = 0.5f;//mMass/(mMass + other->mMass);
        collisionPoint = (1.0f-t)*(nearPoint0 + mRadius*collisionNormal)
                        + t*(nearPoint1 - other.mRadius*collisionNormal);

        return true;
    }

    return false;
}

} // namespace gfx
