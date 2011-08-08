#pragma once

#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>
#include <kgraphics/math/math.h>

namespace gfx {

class Vector3;
class Quat;
class Line3;

/**
 * @class Plane
 *
 */
class Plane
{
public:
    // constructor/destructor
    Plane();
    Plane( float a, float b, float c, float d );
    Plane( const Vector3& p0, const Vector3& p1, const Vector3& p2 );
    inline ~Plane() {}
    
    // copy operations
    Plane(const Plane& other);
    Plane& operator=(const Plane& other);

    // text output (for debugging)
    friend Writer& operator<<(Writer& out, const Plane& source);

    // accessors
    inline const Vector3& GetNormal() const { return mNormal; }
    inline float GetOffset() const { return mOffset; }
    void Get( Vector3& normal, float& direction ) const;

    // comparison
    Bool operator==( const Plane& ray ) const;
    Bool operator!=( const Plane& ray ) const;

    // manipulators
    void Set( float a, float b, float c, float d );
    inline void Set( const Vector3& n, float d ) { Set( n.x, n.y, n.z, d ); }
    void Set( const Vector3& p0, const Vector3& p1, const Vector3& p2 );

    // transform!
    Plane Transform( float scale, const Quat& rotation, 
                       const Vector3& translation ) const;

    // distance
    inline friend float Distance( const Plane& plane, const Vector3& point )
    {
        return Abs( plane.Test( point ) );
    }

    // closest point
    Vector3 ClosestPoint( const Vector3& point ) const;

    // result of plane test
    inline float Test( const Vector3& point ) const
    {
        return mNormal.Dot(point) + mOffset;
    }
        
protected:
    Vector3 	mNormal;
    float     	mOffset;

private:
};

} // namespace gfx
