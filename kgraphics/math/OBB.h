#pragma once 

#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>
#include <kgraphics/math/Matrix33.h>

#include <float.h>

namespace gfx {

class Line3;
class Ray3;
class LineSegment3;
class Plane;

class OBB
{
public:
    // constructor/destructor
    inline OBB() :
        mCenter( 0.0f, 0.0f, 0.0f ), mExtents( 1.0f, 1.0f, 1.0f )
        { mRotation.Identity(); }
    inline ~OBB() {}

    // copy operations
    OBB( const OBB& other );
    OBB& operator=( const OBB& other );

    // text output (for debugging)
    friend Writer& operator<<( Writer& out, const OBB& source );

    // accessors
    inline const Vector3& GetCenter() const   { return mCenter; }
    inline const Matrix33& GetRotation() const { return mRotation; }
    inline const Vector3& GetExtents() const  { return mExtents; }

    // comparison
    bool operator==( const OBB& other ) const;
    bool operator!=( const OBB& other ) const;

    // manipulators
    void Set( const Vector3* points, unsigned int numPoints );
    inline void SetCenter( const Vector3& c ) { mCenter = c; }
    inline void SetRotation( const Matrix33& R ) { mRotation = R; }
    inline void SetExtents( const Vector3& h ) { mExtents = h; }

    // transform!
    OBB Transform( float scale, const Quat& rotation, 
                     const Vector3& translation ) const;
    OBB Transform( float scale, const Matrix33& rotation, 
                     const Vector3& translation ) const;

    // intersection
    bool Intersect( const OBB& other ) const;
    bool Intersect( const Line3& line ) const;
    bool Intersect( const Ray3& ray ) const;
    bool Intersect( const LineSegment3& segment ) const;

    // signed distance to plane
    float Classify( const Plane& plane ) const;

    // merge
    friend void Merge( OBB& result, const OBB& b0, const OBB& b1 );

protected:
    Vector3       mCenter;
    Matrix33      mRotation;
    Vector3       mExtents;

private:
};

} // namespace gfx
