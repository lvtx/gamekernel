#pragma once 

#include <kgraphics/math/math.h>
#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

/**
 * @class Bezier 
 *
 * 베지어 커브 클래스 
 */
class Bezier
{
public:
    // constructor/destructor
    Bezier();
    ~Bezier();

    // text output (for debugging)
    friend Writer& operator<<( Writer& out, const Bezier& source );

    // set up
    Bool Initialize( const Vector3* positions, 
                     const Vector3* controls,
                     const float* times,
                     unsigned int count );

    // automatically create control points
    Bool Initialize( const Vector3* positions, 
                     const float* times,
                     unsigned int count );

    // clean out
    void Clean();

    // evaluate position
    Vector3 Evaluate( float t );

    // evaluate velocity
    Vector3 Velocity( float t );

    // evaluate acceleration
    Vector3 Acceleration( float t );

    // find parameter that moves s distance from Q(t1)
    float FindParameterByDistance( float t1, float s );

    // return length of curve between 1 and u2
    float ArcLength( float t1, float t2 );

    // get total length of curve
    inline float GetLength() { return mTotalLength; }

protected:
    // return length of curve between u1 and u2
    float segmentArcLength( u_int i, float u1, float u2 );
	float SubdivideLength( const Vector3& P0, const Vector3& P1, 
						   const Vector3& P2, const Vector3& P3 );

    Vector3*      mPositions;     // interpolating positions
    Vector3*      mControls;      // approximating positions
    float*          mTimes;         // time to arrive at each point
    float*          mLengths;       // length of each curve segment
    float           mTotalLength;   // total length of curve
    unsigned int    mCount;         // number of points and times

private:
    // copy operations
    // made private so they can't be used
    Bezier( const Bezier& other );
    Bezier& operator=( const Bezier& other );
};

} // namespace gfx
