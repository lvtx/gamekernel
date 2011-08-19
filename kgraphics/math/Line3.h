#pragma once 

#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>
#include <kgraphics/math/math.h>

namespace gfx {

class Vector3;
class Quat;

/**
 * @class Line3
 *
 * ∂Û¿Œ 
 */
class Line3
{
public:
    // constructor/destructor
    Line3();
    Line3( const Vector3& origin, const Vector3& direction );
    inline ~Line3() {}
    
    // copy operations
    Line3(const Line3& other);
    Line3& operator=(const Line3& other);

    // text output (for debugging)
    friend Writer& operator<<(Writer& out, const Line3& source);

    // accessors
    inline Vector3 GetOrigin() const { return mOrigin; }
    inline Vector3 GetDirection() const { return mDirection; }
    void Get( Vector3& origin, Vector3& direction ) const;

    // comparison
    bool operator==( const Line3& line ) const;
    bool operator!=( const Line3& line ) const;

    // manipulators
    void Set( const Vector3& origin, const Vector3& direction );
    inline void Clean() { mOrigin.Clean(); mDirection.Clean(); }

    // transform!
    Line3 Transform( float scale, const Quat& rotation, 
                              const Vector3& translation ) const;

    // distance
    friend float DistanceSquared( const Line3& line0, const Line3& line1, 
                                  float& s_c, float& t_c );
    inline friend float Distance( const Line3& line0, const Line3& line1, 
                           float& s_c, float& t_c )
    {
        return Sqrt( DistanceSquared( line0, line1, s_c, t_c ) );
    }
    friend float DistanceSquared( const Line3& line, const Vector3& point, 
                                  float &t_c );
    inline friend  float Distance( const Line3& line, const Vector3& point, 
                           float &t_c )
    {
        return Sqrt( DistanceSquared( line, point, t_c ) );
    }

	inline bool GetIntersectionWithSphere(Vector3 sorigin, Float sradius, Float& outdistance) const;



    // closest points
    friend void ClosestPoints( Vector3& point0, Vector3& point1, 
                               const Line3& line0, 
                               const Line3& line1 );
    Vector3 ClosestPoint( const Vector3& point ) const;
        
protected:
    Vector3 mOrigin;
    Vector3 mDirection;

private:
};

inline 
bool
Line3::GetIntersectionWithSphere(Vector3 sorigin, Float sradius, Float& outdistance) const
{
	const Vector3 q = sorigin - mOrigin;
	Float c = q.Length();
	Float v = q.Dot(mDirection);
	if ( v < 0 )
	{
		return false;
	}

	Float d = sradius * sradius - (c*c - v*v);

	if (d < 0.0)
		return false;

	outdistance = v - Sqrt ( d );
	return true;
}


} // namespace gfx
