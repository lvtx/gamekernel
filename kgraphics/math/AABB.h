#pragma once 

#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>

#include <float.h>

namespace gfx { 

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

class Line3;
class Ray3;
class LineSegment3;
class Plane;

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class AABB
{
public:
    // constructor/destructor
    inline AABB() :
        mMinima( FLT_MAX, FLT_MAX, FLT_MAX ), mMaxima( FLT_MIN, FLT_MIN, FLT_MIN )
    {}
    inline AABB( const Vector3& min, const Vector3& max ) :
        mMinima( min ), mMaxima( max )
    {}
    inline ~AABB() {}

    // copy operations
    AABB( const AABB& other );
    AABB& operator=( const AABB& other );

    // text output (for debugging)
    friend Writer& operator<<( Writer& out, const AABB& source );

    // accessors
    inline const Vector3& GetMinima() const { return mMinima; }
    inline const Vector3& GetMaxima() const { return mMaxima; }

    // comparison
    bool operator==( const AABB& other ) const;
    bool operator!=( const AABB& other ) const;

    // manipulators
    void Set( const Vector3* points, unsigned int numPoints );
    inline void Set( const Vector3& min, const Vector3& max );
    void AddPoint( const Vector3& point );
	inline Vector3 GetCenter() const;

    // intersection
    bool Intersect( const AABB& other ) const;
    bool Intersect( const Line3& line ) const;
    bool Intersect( const Ray3& ray ) const;
    bool Intersect( const LineSegment3& segment ) const;
	bool isPointInside(const Vector3& p) const;


    // signed distance to plane
    float Classify( const Plane& plane ) const;

    // merge
    friend void Merge( AABB& result, const AABB& b0, const AABB& b1 );


protected:
    Vector3       mMinima, mMaxima;

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------
inline bool
AABB::isPointInside(const Vector3& p) const
{
	return (p.x >= mMinima.x && p.x <= mMaxima.x &&
			p.y >= mMinima.y && p.y <= mMaxima.y &&
			p.z >= mMinima.z && p.z <= mMaxima.z);
}


inline void 
AABB::Set( const Vector3& min, const Vector3& max )
{
    mMinima = min;
    mMaxima = max;
}

inline Vector3
AABB::GetCenter() const
{
	return (mMinima + mMaxima) / 2;
}

//-------------------------------------------------------------------------------
//-- Externs --------------------------------------------------------------------
//-------------------------------------------------------------------------------

} // namespace gfx
