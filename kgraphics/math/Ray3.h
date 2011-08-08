#pragma once 

#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

class Vector3;
class Quat;
class Line3;

class Ray3
{
public:
    // constructor/destructor
    Ray3();
    Ray3( const Vector3& origin, const Vector3& direction );
    inline ~Ray3() {}
    
    // copy operations
    Ray3(const Ray3& other);
    Ray3& operator=(const Ray3& other);

    // text output (for debugging)
    friend Writer& operator<<(Writer& out, const Ray3& source);

    // accessors
    inline Vector3 GetOrigin() const { return mOrigin; }
    inline Vector3 GetDirection() const { return mDirection; }
    void Get( Vector3& origin, Vector3& direction ) const;

    // comparison
    Bool operator==( const Ray3& ray ) const;
    Bool operator!=( const Ray3& ray ) const;

    // manipulators
    void Set( const Vector3& origin, const Vector3& direction );
    inline void Clean() { mOrigin.Clean(); mDirection.Clean(); }

    // transform!
    Ray3 Transform( float scale, const Quat& rotation, 
                              const Vector3& translation ) const;

    // distance
    friend float DistanceSquared( const Ray3& ray0, const Ray3& ray1,
                                  float& s_c, float& t_c );
    friend float DistanceSquared( const Ray3& ray, const Line3& line, 
                                  float& s_c, float& t_c );
    inline friend float DistanceSquared( const Line3& line, const Ray3& ray, 
                                  float& s_c, float& t_c )
    {
        return DistanceSquared( ray, line, t_c, s_c );
    }
    friend float DistanceSquared( const Ray3& ray, const Vector3& point, 
                                  float& t_c );

    // closest points
    friend void ClosestPoints( Vector3& point0, Vector3& point1, 
                               const Ray3& ray0, 
                               const Ray3& ray1 );
    friend void ClosestPoints( Vector3& point0, Vector3& point1, 
                               const Ray3& ray, 
                               const Line3& line );
    Vector3 ClosestPoint( const Vector3& point ) const;
        
protected:
    Vector3 mOrigin;
    Vector3 mDirection;

private:
};

} // namespace gfx
