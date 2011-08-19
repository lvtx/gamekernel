#pragma once 
#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

class Quat;
class Line3;
class Matrix33;
class Ray3;
class LineSegment3;
class Plane;


class BoundingSphere
{
public:
    // constructor/destructor
    inline BoundingSphere() :
        mCenter( 0.0f, 0.0f, 0.0f ), mRadius( 1.0f )
    {}
    inline BoundingSphere( const Vector3& center, float radius ) :
        mCenter( center ), mRadius( radius )
    {}
    inline ~BoundingSphere() {}

    // copy operations
    BoundingSphere( const BoundingSphere& other );
    BoundingSphere& operator=( const BoundingSphere& other );

    // text output (for debugging)
    friend Writer& operator<<( Writer& out, const BoundingSphere& source );

    // accessors
    inline const Vector3& GetCenter() const { return mCenter; }
    inline float GetRadius() const { return mRadius; }

    // comparison
    bool operator==( const BoundingSphere& other ) const;
    bool operator!=( const BoundingSphere& other ) const;

    // manipulators
    inline void SetCenter( const Vector3& center )  { mCenter = center; }
    inline void SetRadius( float radius )  { mRadius = radius; }
    void Set( const Vector3* points, unsigned int numPoints );
    void AddPoint( const Vector3& point );

    // transform!
    BoundingSphere Transform( float scale, const Quat& rotation, 
                         const Vector3& translation ) const;
    BoundingSphere Transform( float scale, const Matrix33& rotation, 
                         const Vector3& translation ) const;

    // intersection
    bool Intersect( const BoundingSphere& other ) const;
    bool Intersect( const Line3& line ) const;
    bool Intersect( const Ray3& ray ) const;
    bool Intersect( const LineSegment3& segment ) const;

    // signed distance to plane
    float Classify( const Plane& plane ) const;

    // merge
    friend void Merge( BoundingSphere& result, 
       const BoundingSphere& s0, const BoundingSphere& s1 );

    // collision parameters
    bool ComputeCollision( const BoundingSphere& other, 
                           Vector3& collisionNormal, 
                           Vector3& collisionPoint, 
                           float& penetration ) const;

protected:
    Vector3       mCenter;
    float           mRadius;

private:
};

} // namespace gfx
