#pragma once 

#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

class Vector3;
class Quat;
class Line3;
class Ray3;
class Matrix33;

/**
 * @class LineSegment3
 *
 */
class LineSegment3
{
public:
    // constructor/destructor
    LineSegment3();
    LineSegment3( const Vector3& endpoint0, const Vector3& endpoint1 );
    inline ~LineSegment3() {}
    
    // copy operations
    LineSegment3(const LineSegment3& other);
    LineSegment3& operator=(const LineSegment3& other);

    // text output (for debugging)
    friend Writer& operator<<(Writer& out, const LineSegment3& source);

    // accessors
    float& operator()(unsigned int i, unsigned int j);
    float  operator()(unsigned int i, unsigned int j) const;

    inline Vector3 GetOrigin() const { return mOrigin; }
    inline Vector3 GetDirection() const { return mDirection; }
    inline Vector3 GetEndpoint0() const { return mOrigin; }
    inline Vector3 GetEndpoint1() const { return mOrigin + mDirection; }
    inline Vector3 GetCenter() const { return mOrigin + 0.5f*mDirection; }
    void Get( Vector3& end0, Vector3& end1 ) const;

    float Length() const;
    float LengthSquared() const;

    // comparison
    bool operator==( const LineSegment3& segment ) const;
    bool operator!=( const LineSegment3& segment ) const;

    // manipulators
    void Set( const Vector3& end0, const Vector3& end1 );
    inline void Clean() { mOrigin.Clean(); mDirection.Clean(); }

    // transform!
    LineSegment3 Transform( float scale, const Quat& rotation, 
                              const Vector3& translation ) const;
    LineSegment3 Transform( float scale, const Matrix33& rotation, 
                              const Vector3& translation ) const;

    // distance
    friend float DistanceSquared( const LineSegment3& segment0, 
                                  const LineSegment3& segment1, 
                                  float& s_c, float& t_c );
    friend float DistanceSquared( const LineSegment3& segment, 
                                  const Ray3& ray, 
                                  float& s_c, float& t_c );
    inline friend float DistanceSquared( const Ray3& ray, 
                                  const LineSegment3& segment, 
                                  float& s_c, float& t_c )
    {
        return DistanceSquared( segment, ray, t_c, s_c );
    }
    friend float DistanceSquared( const LineSegment3& segment, 
                                  const Line3& line, 
                                  float& s_c, float& t_c );
    inline friend float DistanceSquared( const Line3& line, 
                                  const LineSegment3& segment, 
                                  float& s_c, float& t_c )
    {
        return DistanceSquared( segment, line, t_c, s_c );
    }
    friend float DistanceSquared( const LineSegment3& segment, 
                                  const Vector3& point, 
                                  float& t_c );

    // closest points
    friend void ClosestPoints( Vector3& point0, Vector3& point1, 
                               const LineSegment3& segment0, 
                               const LineSegment3& segment1 );
    friend void ClosestPoints( Vector3& point0, Vector3& point1, 
                               const LineSegment3& segment, 
                               const Ray3& ray );
    friend void ClosestPoints( Vector3& point0, Vector3& point1, 
                               const LineSegment3& segment, 
                               const Line3& line );
    Vector3 ClosestPoint( const Vector3& point ) const;
        
protected:
    Vector3 mOrigin;
    Vector3 mDirection;

private:
};

} // namespace gfx
