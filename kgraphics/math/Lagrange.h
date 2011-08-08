#pragma once

#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

/**
 * @class Lagrange
 *
 * Piecewise Lagrangely interpolated spline
 */
class Lagrange
{
public:
    // constructor/destructor
    Lagrange();
    ~Lagrange();

    // text output (for debugging)
    friend Writer& operator<<( Writer& out, const Lagrange& source );

    // set up
    Bool Initialize( const Vector3* samples, const float* times,
                     unsigned int count );

    // destroy
    void Clean();

    // evaluate position
    Vector3 Evaluate( float t );

protected:
    Vector3*      	mPositions; // sample points
    float*          mTimes;     // time to arrive at each point
    float*          mDenomRecip;// 1/lagrange denomenator for each point
    unsigned int    mCount;     // number of points and times

private:
    // copy operations
    // made private so they can't be used
    Lagrange( const Lagrange& other );
    Lagrange& operator=( const Lagrange& other );
};

} // namespace gfx
