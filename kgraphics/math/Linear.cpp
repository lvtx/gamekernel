#include <stdafx.h>

#include <kgraphics/math/Linear.h>

namespace gfx {

//-------------------------------------------------------------------------------
// @ Linear::Linear()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
Linear::Linear() :
    mPositions( 0 ),
    mTimes( 0 ),
    mCount( 0 )
{
}   // End of Linear::Linear()


//-------------------------------------------------------------------------------
// @ Linear::Linear()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Linear::~Linear()
{
    delete [] mPositions;
    delete [] mTimes;

}   // End of Linear::~Linear()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const Linear& source)
{
    out << source.mCount << eol;
    for (unsigned int i = 0; i < source.mCount; ++i )
    {
        out << source.mTimes[i] << ':' << source.mPositions[i] << eol;
    }

    return out;
    
}   // End of operator<<()


//-------------------------------------------------------------------------------
// @ Linear::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
bool
Linear::Initialize( const Vector3* samples, const float* times,
                      unsigned int count )
{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 2 || !samples || !times )
        return false;

    // set up arrays
    mPositions = new Vector3[count];
    mTimes = new float[count];
    mCount = count;

    // copy data
    for ( unsigned int i = 0; i < count; ++i )
    {
        mPositions[i] = samples[i];
        mTimes[i] = times[i];
    }

    return true;

}   // End of Linear::Initialize()


//-------------------------------------------------------------------------------
// @ Linear::Clean()
//-------------------------------------------------------------------------------
// Clean out data
//-------------------------------------------------------------------------------
void 
Linear::Clean()
{
    delete [] mPositions;
    delete [] mTimes;
    mCount = 0;

}   // End of Linear::Clean()


//-------------------------------------------------------------------------------
// @ Linear::Evaluate()
//-------------------------------------------------------------------------------
// Evaluate spline
//-------------------------------------------------------------------------------
Vector3
Linear::Evaluate( float t )
{
    // make sure data is valid
    //ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return Vector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        return mPositions[0];
    else if ( t >= mTimes[mCount-1] )
        return mPositions[mCount-1];

    // find segment and parameter
    unsigned int i;
    for ( i = 0; i < mCount-1; ++i )
    {
        if ( t < mTimes[i+1] )
        {
            break;
        }
    }
    float t0 = mTimes[i];
    float t1 = mTimes[i+1];
    float u = (t - t0)/(t1 - t0);

    // evaluate
    return (1-u)*mPositions[i] + u*mPositions[i+1];

}   // End of Linear::Initialize()

} // namespace gfx
