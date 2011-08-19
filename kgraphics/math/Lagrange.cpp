#include <stdafx.h>

#include <kgraphics/math/Lagrange.h>

namespace gfx {

//-------------------------------------------------------------------------------
// @ Lagrange::Lagrange()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
Lagrange::Lagrange() :
    mPositions( 0 ),
    mTimes( 0 ),
    mDenomRecip( 0 ),
    mCount( 0 )
{
}   // End of Lagrange::Lagrange()


//-------------------------------------------------------------------------------
// @ Lagrange::Lagrange()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Lagrange::~Lagrange()
{
    delete [] mPositions;
    delete [] mTimes;
    delete [] mDenomRecip;

}   // End of Lagrange::~Lagrange()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const Lagrange& source)
{
    out << source.mCount << eol;
    for (unsigned int i = 0; i < source.mCount; ++i )
    {
        out << source.mTimes[i] << ':' << source.mPositions[i] << eol;
    }

    return out;
    
}   // End of operator<<()


//-------------------------------------------------------------------------------
// @ Lagrange::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
bool
Lagrange::Initialize( const Vector3* samples, const float* times,
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
    mDenomRecip = new float[count];
    mCount = count;

    // copy data
    for ( unsigned int k = 0; k < count; ++k )
    {
        mPositions[k] = samples[k];
        mTimes[k] = times[k];
        float denom = 1.0f;
        for ( unsigned int i = 0; i < count; ++i )
        {
            if (k != i)
            {
                denom *= (times[k] - times[i]);
            }
        }
        mDenomRecip[k] = 1.0f/denom;
    }

    return true;

}   // End of Lagrange::Initialize()


//-------------------------------------------------------------------------------
// @ Lagrange::Clean()
//-------------------------------------------------------------------------------
// Clean out data
//-------------------------------------------------------------------------------
void 
Lagrange::Clean()
{
    delete [] mPositions;
    delete [] mTimes;
    delete [] mDenomRecip;
    mCount = 0;

}   // End of Lagrange::Clean()


//-------------------------------------------------------------------------------
// @ Lagrange::Evaluate()
//-------------------------------------------------------------------------------
// Evaluate curve
//-------------------------------------------------------------------------------
Vector3
Lagrange::Evaluate( float t )
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

    // evaluate
    Vector3 result(0.0f, 0.0f, 0.0f);
    for ( unsigned int k = 0; k < mCount; ++k )
    {
        float num = 1.0f;
        for ( unsigned int i = 0; i < mCount; ++i )
        {
            if (k != i)
            {
                num *= (t - mTimes[i]);
            }
        }

        result += num*mDenomRecip[k]*mPositions[k];
    }

    return result;

}   // End of Lagrange::Evaluate ()


} // namespace gfx
