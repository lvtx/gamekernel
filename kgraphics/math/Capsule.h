#pragma once 

#include <kgraphics/math/Writer.h>
#include <kgraphics/math/LineSegment3.h>

namespace gfx { 

class Vector3;
class Line3;
class Matrix33;
class Ray3;
class Plane;

/**
 * @class Capsule 
 *
 * Ä¸½¶ ÇüÅÂÀÇ ¹Ù¿îµù º¼·ý
 */
class Capsule
{
public:
    // constructor/destructor
    inline Capsule() {}
    inline Capsule( const Vector3& endpoint0, const Vector3& endpoint1, float radius ) :
        mSegment( endpoint0, endpoint1 ), mRadius( radius )
    {
    }
    inline Capsule( const LineSegment3& segment, float radius ) :
        mSegment( segment ), mRadius( radius )
    {
    }
    inline ~Capsule() {}

    // copy operations
    Capsule( const Capsule& other );
    Capsule& operator=( const Capsule& other );

    // text output (for debugging)
    friend Writer& operator<<( Writer& out, const Capsule& source );

    // accessors
    inline const LineSegment3& GetSegment() const { return mSegment; }
    inline float GetRadius() const { return mRadius; }

    float Length() const;
    float LengthSquared() const;

    // comparison
    bool operator==( const Capsule& other ) const;
    bool operator!=( const Capsule& other ) const;

    // manipulators
    inline void SetSegment( const LineSegment3& segment )  { mSegment = segment; }
    inline void SetRadius( float radius )  { mRadius = radius; }

    void Set( const Vector3* points, unsigned int numPoints );

    // transform!
    Capsule Transform( float scale, const Quat& rotation, 
                         const Vector3& translation ) const;
    Capsule Transform( float scale, const Matrix33& rotation, 
                         const Vector3& translation ) const;

    // intersection
    bool Intersect( const Capsule& other ) const;
    bool Intersect( const Line3& line ) const;
    bool Intersect( const Ray3& ray ) const;
    bool Intersect( const LineSegment3& segment ) const;

    // signed distance to plane
    float Classify( const Plane& plane ) const;

    // merge 
    friend void Merge( Capsule& result, const Capsule& b0, const Capsule& b1 );

    // collision parameters
    bool ComputeCollision( const Capsule& other, 
                           Vector3& collisionNormal, 
                           Vector3& collisionPoint, 
                           float& penetration ) const;

protected:
    LineSegment3  mSegment;
    float           mRadius;

private:
};

} // namespace gfx
