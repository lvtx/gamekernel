#include <stdafx.h>

#include <kgraphics/math/Sphere.h>
#include <kgraphics/math/Vector3.h>
#include <kgraphics/math/Line3.h>
#include <kgraphics/math/Matrix33.h>
#include <kgraphics/math/Ray3.h>
#include <kgraphics/math/LineSegment3.h>
#include <kgraphics/math/math.h>
#include <kgraphics/math/Plane.h>
#include <kgraphics/math/Quat.h>

namespace gfx { 

//-------------------------------------------------------------------------------
// @ BoundingSphere::BoundingSphere()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
BoundingSphere::BoundingSphere(const BoundingSphere& other) : 
    mCenter( other.mCenter ),
    mRadius( other.mRadius )
{

}   // End of BoundingSphere::BoundingSphere()


//-------------------------------------------------------------------------------
// @ BoundingSphere::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
BoundingSphere&
BoundingSphere::operator=(const BoundingSphere& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    mCenter = other.mCenter;
    mRadius = other.mRadius;
    
    return *this;

}   // End of BoundingSphere::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const BoundingSphere& source)
{
    out << source.mCenter;
    out << ' ' << source.mRadius;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ BoundingSphere::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
BoundingSphere::operator==( const BoundingSphere& other ) const
{
    if ( other.mCenter == mCenter && other.mRadius == mRadius )
        return true;

    return false;   
}   // End of BoundingSphere::operator==()


//-------------------------------------------------------------------------------
// @ BoundingSphere::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
BoundingSphere::operator!=( const BoundingSphere& other ) const
{
    if ( other.mCenter != mCenter || other.mRadius != mRadius )
        return true;

    return false;

}   // End of BoundingSphere::operator!=()


//-------------------------------------------------------------------------------
// @ BoundingSphere::Set()
//-------------------------------------------------------------------------------
// Set bounding sphere based on set of points
//-------------------------------------------------------------------------------
void
BoundingSphere::Set( const Vector3* points, unsigned int numPoints )
{
    ////ASSERT( points );
    // compute minimal and maximal bounds
    Vector3 min(points[0]), max(points[0]);
    unsigned int i;
    for ( i = 1; i < numPoints; ++i )
    {
        if (points[i].x < min.x)
            min.x = points[i].x;
        else if (points[i].x > max.x )
            max.x = points[i].x;
        if (points[i].y < min.y)
            min.y = points[i].y;
        else if (points[i].y > max.y )
            max.y = points[i].y;
        if (points[i].z < min.z)
            min.z = points[i].z;
        else if (points[i].z > max.z )
            max.z = points[i].z;
    }
    // compute center and radius
    mCenter = 0.5f*(min + max);
    float maxDistance = gfx::DistanceSquared( mCenter, points[0] );
    for ( i = 1; i < numPoints; ++i )
    {
        float dist = gfx::DistanceSquared( mCenter, points[i] );
        if (dist > maxDistance)
            maxDistance = dist;
    }
    mRadius = Sqrt( maxDistance );
}


//----------------------------------------------------------------------------
// @ BoundingSphere::Transform()
// ---------------------------------------------------------------------------
// Transforms sphere into new space
//-----------------------------------------------------------------------------
BoundingSphere    
BoundingSphere::Transform( float scale, const Quat& rotate, 
    const Vector3& translate ) const
{
    return BoundingSphere( rotate.Rotate(mCenter) + translate, mRadius*scale );

}   // End of BoundingSphere::Transform()


//----------------------------------------------------------------------------
// @ BoundingSphere::Transform()
// ---------------------------------------------------------------------------
// Transforms sphere into new space
//-----------------------------------------------------------------------------
BoundingSphere    
BoundingSphere::Transform( float scale, const Matrix33& rotate, 
    const Vector3& translate ) const
{
    return BoundingSphere( rotate*mCenter + translate, mRadius*scale );

}   // End of BoundingSphere::Transform()


//----------------------------------------------------------------------------
// @ BoundingSphere::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between sphere and sphere
//-----------------------------------------------------------------------------
bool 
BoundingSphere::Intersect( const BoundingSphere& other ) const
{
    // do sphere check
    float radiusSum = mRadius + other.mRadius;
    Vector3 centerDiff = other.mCenter - mCenter; 
    float distancesq = centerDiff.LengthSquared();

    // if distance squared < sum of radii squared, collision!
    return ( distancesq <= radiusSum*radiusSum );
}


//----------------------------------------------------------------------------
// @ BoundingSphere::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between sphere and line
//-----------------------------------------------------------------------------
bool
BoundingSphere::Intersect( const Line3& line ) const
{
    // compute intermediate values
    Vector3 w = mCenter - line.GetOrigin();
    float wsq = w.Dot(w);
    float proj = w.Dot(line.GetDirection());
    float rsq = mRadius*mRadius;
    float vsq = line.GetDirection().Dot(line.GetDirection());

    // test length of difference vs. radius
    return ( vsq*wsq - proj*proj <= vsq*rsq );
}


//----------------------------------------------------------------------------
// @ BoundingSphere::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between sphere and ray
//-----------------------------------------------------------------------------
bool
BoundingSphere::Intersect( const Ray3& ray ) const
{
    // compute intermediate values
    Vector3 w = mCenter - ray.GetOrigin();
    float wsq = w.Dot(w);
    float proj = w.Dot(ray.GetDirection());
    float rsq = mRadius*mRadius;

    // if sphere behind ray, no intersection
    if ( proj < 0.0f && wsq > rsq )
        return false;
    float vsq = ray.GetDirection().Dot(ray.GetDirection());

    // test length of difference vs. radius
    return ( vsq*wsq - proj*proj <= vsq*rsq );
}


//----------------------------------------------------------------------------
// @ BoundingSphere::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between sphere and line segment
//-----------------------------------------------------------------------------
bool
BoundingSphere::Intersect( const LineSegment3& segment ) const
{
    // compute intermediate values
    Vector3 w = mCenter - segment.GetOrigin();
    float wsq = w.Dot(w);
    float proj = w.Dot(segment.GetDirection());
    float rsq = mRadius*mRadius;

    // if sphere outside segment, no intersection
    if ( (proj < 0.0f || proj > 1.0f) && wsq > rsq )
        return false;
    float vsq = segment.GetDirection().Dot(segment.GetDirection());

    // test length of difference vs. radius
    return ( vsq*wsq - proj*proj <= vsq*rsq );
}


//----------------------------------------------------------------------------
// @ BoundingSphere::Classify()
// ---------------------------------------------------------------------------
// Compute signed distance between sphere and plane
//-----------------------------------------------------------------------------
float
BoundingSphere::Classify( const Plane& plane ) const
{
    float distance = plane.Test( mCenter );
    if ( distance > mRadius )
    {
        return distance-mRadius;
    }
    else if ( distance < -mRadius )
    {
        return distance+mRadius;
    }
    else
    {
        return 0.0f;
    }

}   // End of BoundingSphere::Classify()



//----------------------------------------------------------------------------
// @ ::Merge()
// ---------------------------------------------------------------------------
// Merge two spheres together to create a new one
//-----------------------------------------------------------------------------
void
Merge( BoundingSphere& result, 
       const BoundingSphere& s0, const BoundingSphere& s1 )
{
    // get differences between them
    Vector3 diff = s1.mCenter - s0.mCenter;
    float distsq = diff.Dot(diff);
    float radiusdiff = s1.mRadius - s0.mRadius;

    // if one sphere inside other
    if ( distsq <= radiusdiff*radiusdiff )
    {
        if ( s0.mRadius > s1.mRadius )
            result = s0;
        else
            result = s1;
        return;
    }

    // build new sphere
    float dist = Sqrt( distsq );
    float radius = 0.5f*( s0.mRadius + s1.mRadius + dist );
    Vector3 center = s0.mCenter;
    if (!gfx::IsZero( dist ))
        center += ((radius-s0.mRadius)/dist)*diff;

    result.SetRadius( radius );
    result.SetCenter( center );

}   // End of ::Merge()


//----------------------------------------------------------------------------
// @ BoundingSphere::ComputeCollision()
// ---------------------------------------------------------------------------
// Compute parameters for collision between sphere and sphere
//-----------------------------------------------------------------------------
bool 
BoundingSphere::ComputeCollision( const BoundingSphere& other, Vector3& collisionNormal, 
                      Vector3& collisionPoint, float& penetration ) const
{
    // do sphere check
    float radiusSum = mRadius + other.mRadius;
    collisionNormal = other.mCenter - mCenter;  
    float distancesq = collisionNormal.LengthSquared();
    // if distance squared < sum of radii squared, collision!
    if ( distancesq <= radiusSum*radiusSum )
    {
        // handle collision

        // penetration is distance - radii
        float distance = Sqrt(distancesq);
        penetration = radiusSum - distance;
        collisionNormal.Normalize();

        // collision point is average of penetration
        collisionPoint = 0.5f*(mCenter + mRadius*collisionNormal) 
                        + 0.5f*(other.mCenter - other.mRadius*collisionNormal);

        return true;
    }

    return false;

}  // End of ::ComputeCollision()

} // namespace gfx
