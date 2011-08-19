#pragma once 

#include <kgraphics/math/math.h>
#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

/**
 * @class UniformBSpline
 *
 * Uniform B-spline class
 */
class UniformBSpline
{
public:
    // constructor/destructor
    UniformBSpline();
    ~UniformBSpline();

    // text output (for debugging)
    friend Writer& operator<<( Writer& out, const UniformBSpline& source );

    // set up
    bool Initialize( const Vector3* positions, 
                     unsigned int count,
                     float startTime, float endTime );

    // remove data
    void Clean();

    // evaluate position
    Vector3 Evaluate( float t );

    // evaluate derivative
    Vector3 Velocity( float t );

    // evaluate second derivative
    Vector3 Acceleration( float t );

    // find parameter that moves s distance from Q(t1)
    float FindParameterByDistance( float t1, float s );

    // return length of curve between t1 and t2
    float ArcLength( float t1, float t2 );

    // get total length of curve
    inline float GetLength() { return mTotalLength; }

protected:
    // return length of curve between u1 and u2
    float SegmentArcLength( u_int i, float u1, float u2 );

    Vector3*      	mPositions;     // positions of control points
    float*          mTimes;         // time to approach each point
    float*          mLengths;       // length of each curve segment
    float           mTotalLength;   // total length of curve
    unsigned int    mCount;         // number of points and times

private:
    // copy operations
    // made private so they can't be used
    UniformBSpline( const UniformBSpline& other );
    UniformBSpline& operator=( const UniformBSpline& other );
};

} // namespace gfx
