#pragma once 

#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

/**
 * @class LInear
 *
 * Piecewise linearly interpolated spline
 */
class Linear
{
public:
    // constructor/destructor
    Linear();
    ~Linear();

    // text output (for debugging)
    friend Writer& operator<<( Writer& out, const Linear& source );

    // set up
    Bool Initialize( const Vector3* samples, const float* times,
                     unsigned int count );

    // clean up
    void Clean();

    // evaluate position
    Vector3 Evaluate( float t );

protected:
    Vector3*      mPositions; // sample points
    float*          mTimes;     // time to arrive at each point
    unsigned int    mCount;     // number of points and times

private:
    // copy operations
    // made private so they can't be used
    Linear( const Linear& other );
    Linear& operator=( const Linear& other );
};

} // namespace gfx
