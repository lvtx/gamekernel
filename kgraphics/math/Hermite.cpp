#include <stdafx.h>

#include <kgraphics/math/Hermite.h>
#include <kgraphics/math/GaussianElim.h>
#include <kgraphics/math/math.h>
#include <string.h>
#include <float.h>

namespace gfx {

//-------------------------------------------------------------------------------
// @ Hermite::Hermite()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
Hermite::Hermite() :
    mPositions( 0 ),
    mInTangents( 0 ),
    mOutTangents( 0 ),
    mTimes( 0 ),
    mLengths( 0 ),
    mTotalLength( 0.0f ),
    mCount( 0 )
{
}   // End of Hermite::Hermite()


//-------------------------------------------------------------------------------
// @ Hermite::Hermite()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Hermite::~Hermite()
{
    Clean();

}   // End of Hermite::~Hermite()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const Hermite& source)
{
    out << source.mCount << eol;
    for (unsigned int i = 0; i < source.mCount; ++i )
    {
        out << source.mTimes[i] << ':' << source.mPositions[i];
        if ( i < source.mCount-1 )
            out << ',' << source.mInTangents[i];
        if ( i > 0 )
            out << ',' << source.mOutTangents[i-1];
        out << eol;
    }

    return out;
    
}   // End of operator<<()


//-------------------------------------------------------------------------------
// @ Hermite::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
Bool
Hermite::Initialize( const Vector3* positions,
                       const Vector3* inTangents,
                       const Vector3* outTangents,
                       const float* times,
                       unsigned int count )

{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 2 || !positions || !times || !inTangents || !outTangents )
        return false;

    // set up arrays
    mPositions = new Vector3[count];
    mInTangents = new Vector3[count-1];
    mOutTangents = new Vector3[count-1];
    mTimes = new float[count];
    mCount = count;

    // copy data
    unsigned int i;
    for ( i = 0; i < count; ++i )
    {
        mPositions[i] = positions[i];
        if ( i < count-1 )
        {
            mInTangents[i] = inTangents[i];
            mOutTangents[i] = outTangents[i];
        }
        mTimes[i] = times[i];
    }

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( i = 0; i < count-1; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    return true;

}   // End of Hermite::Initialize()


//-------------------------------------------------------------------------------
// @ Hermite::InitializeClamped()
//-------------------------------------------------------------------------------
// Set up sample points for clamped spline
//-------------------------------------------------------------------------------
Bool 
Hermite::InitializeClamped( const Vector3* positions, 
                              const float* times,
                              unsigned int count,
                              const Vector3& inTangent,
                              const Vector3& outTangent )
{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 3 || !positions || !times )
        return false;

    // build A
    unsigned int n = count;
    float* A = new float[n*n];
    memset(A, 0, sizeof(float)*n*n);

    A[0] = 1.0f;
    unsigned int i;
    for ( i = 1; i < n-1; ++i )
    {
        A[i + n*i - n] = 1.0f;
        A[i + n*i] = 4.0f;
        A[i + n*i + n] = 1.0f;
    }
    A[n*n-1] = 1.0f;
    
    // invert it
    // we'd might get better accuracy if we solve the linear system 3 times,
    // once each for x, y, and z, but this is more efficient
    if (!gfx::InvertMatrix( A, n ))
    {
        delete [] A;
        return false;
    }

    // set up arrays
    mPositions = new Vector3[count];
    mInTangents = new Vector3[count-1];
    mOutTangents = new Vector3[count-1];
    mTimes = new float[count];
    mCount = count;

    // handle end conditions
    mPositions[0] = positions[0];
    mTimes[0] = times[0];
    mInTangents[0] = inTangent;
    mPositions[count-1] = positions[count-1];
    mTimes[count-1] = times[count-1];
    mOutTangents[count-2] = outTangent;

    // set up the middle
    for ( i = 1; i < count-1; ++i )
    {
        // copy position and time
        mPositions[i] = positions[i];
        mTimes[i] = times[i];

        // multiply b by inverse of A to get x
        mInTangents[i] = A[i]*inTangent + A[i + n*n-n]*outTangent;
        for ( u_int j = 1; j < n-1; ++j )
        {
            Vector3 b_j = 3.0f*(positions[j+1]-positions[j-1]);
            mInTangents[i] += A[i + n*j]*b_j;
        }

        // out tangent is in tangent of next segment
        mOutTangents[i-1] = mInTangents[i];
    }

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( i = 0; i < count-1; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    delete [] A;

    return true;

}   // End of Hermite::InitializeClamped()


//-------------------------------------------------------------------------------
// @ Hermite::InitializeNatural()
//-------------------------------------------------------------------------------
// Set up sample points for natural spline
//-------------------------------------------------------------------------------
Bool 
Hermite::InitializeNatural( const Vector3* positions, 
                              const float* times,
                              unsigned int count )
{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 3 || !positions || !times )
        return false;

    // build A
    unsigned int n = count;
    float* A = new float[n*n];
    memset(A, 0, sizeof(float)*n*n);

    A[0] = 2.0f;
    A[n] = 1.0f;
    unsigned int i;
    for ( i = 1; i < n-1; ++i )
    {
        A[i + n*i - n] = 1.0f;
        A[i + n*i] = 4.0f;
        A[i + n*i + n] = 1.0f;
    }
    A[n*(n-1)-1] = 1.0f;
    A[n*n-1] = 2.0f;
    
    // invert it
    // we'd might get better accuracy if we solve the linear system 3 times,
    // once each for x, y, and z, but this is more efficient
    if (!gfx::InvertMatrix( A, n ))
    {
        delete [] A;
        return false;
    }

    // set up arrays
    mPositions = new Vector3[count];
    mInTangents = new Vector3[count-1];
    mOutTangents = new Vector3[count-1];
    mTimes = new float[count];
    mCount = count;

    // set up the tangents
    for ( i = 0; i < count; ++i )
    {
        // copy position and time
        mPositions[i] = positions[i];
        mTimes[i] = times[i];

        // multiply b by inverse of A to get tangents
        // compute count-1 incoming tangents
        if ( i < count-1 )
        {
            mInTangents[i] = 3.0f*A[i]*(positions[1]-positions[0])
                             + 3.0f*A[i + n*n-n]*(positions[n-1]-positions[n-2]);
            for ( u_int j = 1; j < n-1; ++j )
            {
                Vector3 b_j = 3.0f*(positions[j+1]-positions[j-1]);
                mInTangents[i] += A[i + n*j]*b_j;
            }
            // out tangent is in tangent of next segment
            if (i > 0)
                mOutTangents[i-1] = mInTangents[i];
        }
        // compute final outgoing tangent
        else
        {
            mOutTangents[i-1] = 3.0f*A[i]*(positions[1]-positions[0])
                             + 3.0f*A[i + n*n-n]*(positions[n-1]-positions[n-2]);
            for ( u_int j = 1; j < n-1; ++j )
            {
                Vector3 b_j = 3.0f*(positions[j+1]-positions[j-1]);
                mOutTangents[i-1] += A[i + n*j]*b_j;
            }
        }
    }

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( i = 0; i < count-1; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    delete [] A;

    return true;

}   // End of Hermite::InitializeNatural()


//-------------------------------------------------------------------------------
// @ Hermite::InitializeCyclic()
//-------------------------------------------------------------------------------
// Set up sample points for cyclic spline
//-------------------------------------------------------------------------------
Bool 
Hermite::InitializeCyclic( const Vector3* positions, 
                              const float* times,
                              unsigned int count )
{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 4 || !positions || !times )
        return false;

    // build A
    unsigned int n = count-1;
    float* A = new float[n*n];
    memset(A, 0, sizeof(float)*n*n);

    A[0] = 4.0f;
    A[n] = 1.0f;
    A[n*n-n] = 1.0f;
    unsigned int i;
    for ( i = 1; i < n-1; ++i )
    {
        A[i + n*i - n] = 1.0f;
        A[i + n*i] = 4.0f;
        A[i + n*i + n] = 1.0f;
    }
    A[n*(n-2)-1] = 1.0f;
    A[n*(n-1)-1] = 1.0f;
    A[n*n-1] = 4.0f;
    
    // invert it
    // we'd might get better accuracy if we solve the linear system 3 times,
    // once each for x, y, and z, but this is more efficient
    if (!gfx::InvertMatrix( A, n ))
    {
        delete [] A;
        return false;
    }

    // set up arrays
    mPositions = new Vector3[count];
    mInTangents = new Vector3[count-1];
    mOutTangents = new Vector3[count-1];
    mTimes = new float[count];
    mCount = count;

    Vector3 b0 = 3.0f*(positions[1]-positions[0] + positions[count-1]-positions[count-2]);

    // set up the tangents
    for ( i = 0; i < count; ++i )
    {
        // copy position and time
        mPositions[i] = positions[i];
        mTimes[i] = times[i];

        // multiply b by inverse of A to get tangents
        // compute count-1 incoming tangents
        if ( i < count-1 )
        {
            mInTangents[i] = A[i]*b0;
            for ( u_int j = 1; j < n; ++j )
            {
                Vector3 b_j = 3.0f*(positions[j+1]-positions[j-1]);
                mInTangents[i] += A[i + n*j]*b_j;
            }
            // outgoing tangent is incoming tangent of next segment
            if (i > 0)
                mOutTangents[i-1] = mInTangents[i];
        }
    }
    mOutTangents[count-2] = mInTangents[0];

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( i = 0; i < count-1; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    delete [] A;

    return true;

}   // End of Hermite::InitializeCyclic()


//-------------------------------------------------------------------------------
// @ Hermite::InitializeAcyclic()
//-------------------------------------------------------------------------------
// Set up sample points for cyclic spline
//-------------------------------------------------------------------------------
Bool 
Hermite::InitializeAcyclic( const Vector3* positions, 
                              const float* times,
                              unsigned int count )
{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 4 || !positions || !times )
        return false;

    // build A
    unsigned int n = count-1;
    float* A = new float[n*n];
    memset(A, 0, sizeof(float)*n*n);

    A[0] = 4.0f;
    A[n] = 1.0f;
    A[n*n-n] = -1.0f;
    unsigned int i;
    for ( i = 1; i < n-1; ++i )
    {
        A[i + n*i - n] = 1.0f;
        A[i + n*i] = 4.0f;
        A[i + n*i + n] = 1.0f;
    }
    A[n*(n-2)-1] = -1.0f;
    A[n*(n-1)-1] = 1.0f;
    A[n*n-1] = 4.0f;
    
    // invert it
    // we'd might get better accuracy if we solve the linear system 3 times,
    // once each for x, y, and z, but this is more efficient
    if (!gfx::InvertMatrix( A, n ))
    {
        delete [] A;
        return false;
    }

    // set up arrays
    mPositions = new Vector3[count];
    mInTangents = new Vector3[count-1];
    mOutTangents = new Vector3[count-1];
    mTimes = new float[count];
    mCount = count;

    Vector3 b0 = 3.0f*(positions[1]-positions[0] - positions[count-1]+positions[count-2]);

    // set up the tangents
    for ( i = 0; i < count; ++i )
    {
        // copy position and time
        mPositions[i] = positions[i];
        mTimes[i] = times[i];

        // multiply b by inverse of A to get tangents
        // compute count-1 incoming tangents
        if ( i < count-1 )
        {
            mInTangents[i] = A[i]*b0;
            for ( u_int j = 1; j < n; ++j )
            {
                Vector3 b_j = 3.0f*(positions[j+1]-positions[j-1]);
                mInTangents[i] += A[i + n*j]*b_j;
            }
            // outgoing tangent is incoming tangent of next segment
            if (i > 0)
                mOutTangents[i-1] = mInTangents[i];
        }
    }
    mOutTangents[count-2] = -mInTangents[0];

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( i = 0; i < count-1; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    delete [] A;

    return true;

}   // End of Hermite::InitializeAcyclic()


//-------------------------------------------------------------------------------
// @ Hermite::Clean()
//-------------------------------------------------------------------------------
// Clean out data
//-------------------------------------------------------------------------------
void Hermite::Clean()
{
    delete [] mPositions;
    delete [] mInTangents;
    delete [] mOutTangents;
    delete [] mTimes;
    delete [] mLengths;
    mTotalLength = 0.0f;
    mCount = 0;

}   // End of Hermite::Clean()


//-------------------------------------------------------------------------------
// @ Hermite::Evaluate()
//-------------------------------------------------------------------------------
// Evaluate spline
//-------------------------------------------------------------------------------
Vector3
Hermite::Evaluate( float t )
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
    Vector3 A = 2.0f*mPositions[i]
                - 2.0f*mPositions[i+1]
                + mInTangents[i]
                + mOutTangents[i];
    Vector3 B = -3.0f*mPositions[i]
                + 3.0f*mPositions[i+1]
                - 2.0f*mInTangents[i]
                - mOutTangents[i];
    
    return mPositions[i] + u*(mInTangents[i] + u*(B + u*A));

}   // End of Hermite::Evaluate()


//-------------------------------------------------------------------------------
// @ Hermite::Velocity()
//-------------------------------------------------------------------------------
// Evaluate derivative at parameter t
//-------------------------------------------------------------------------------
Vector3
Hermite::Velocity( float t )
{
    // make sure data is valid
    //ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return Vector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        return mInTangents[0];
    else if ( t >= mTimes[mCount-1] )
        return mOutTangents[mCount-2];

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
    Vector3 A = 2.0f*mPositions[i]
                - 2.0f*mPositions[i+1]
                + mInTangents[i]
                + mOutTangents[i];
    Vector3 B = -3.0f*mPositions[i]
                + 3.0f*mPositions[i+1]
                - 2.0f*mInTangents[i]
                - mOutTangents[i];
    
    return mInTangents[i] + u*(2.0f*B + 3.0f*u*A);

}   // End of Hermite::Velocity()


//-------------------------------------------------------------------------------
// @ Hermite::Acceleration()
//-------------------------------------------------------------------------------
// Evaluate second derivative at parameter t
//-------------------------------------------------------------------------------
Vector3
Hermite::Acceleration( float t )
{
    // make sure data is valid
    //ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return Vector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        t = 0.0f;
    else if ( t > mTimes[mCount-1] )
        t = mTimes[mCount-1];

    // find segment and parameter
    unsigned int i;
    for ( i = 0; i < mCount-1; ++i )
    {
        if ( t <= mTimes[i+1] )
        {
            break;
        }
    }
    float t0 = mTimes[i];
    float t1 = mTimes[i+1];
    float u = (t - t0)/(t1 - t0);

    // evaluate
    Vector3 A = 2.0f*mPositions[i]
                - 2.0f*mPositions[i+1]
                + mInTangents[i]
                + mOutTangents[i];
    Vector3 B = -3.0f*mPositions[i]
                + 3.0f*mPositions[i+1]
                - 2.0f*mInTangents[i]
                - mOutTangents[i];
    
    return 2.0f*B + 6.0f*u*A;

}   // End of Hermite::Acceleration()


//-------------------------------------------------------------------------------
// @ Hermite::FindParameterByDistance()
//-------------------------------------------------------------------------------
// Find parameter s distance in arc length from Q(t1)
// Returns max float if can't find it
//
// This extends the approach in the text and uses a mixture of bisection and 
// Newton-Raphson to find the root.  The result is more stable than Newton-
// Raphson alone because a) we won't end up with a situation where we divide by 
// zero in the Newton-Raphson step and b) the end result converges faster.
//
// See Numerical Recipes or http://www.essentialmath.com/blog for more details.
//-------------------------------------------------------------------------------
float 
Hermite::FindParameterByDistance( float t1, float s )
{
    // initialize bisection endpoints
    float a = t1;
    float b = mTimes[mCount-1];

    // ensure that we remain within valid parameter space
    if ( s >= ArcLength(t1, b) )
        return b;
    if ( s <= 0.0f )
        return a;

    // make first guess
    float p = t1 + s*(mTimes[mCount-1]-mTimes[0])/mTotalLength;

    // iterate and look for zeros
    for ( u_int i = 0; i < 32; ++i )
    {
        // compute function value and test against zero
        float func = ArcLength(t1, p) - s;
        if ( gfx::Abs(func) < 1.0e-03f )
        {
            return p;
        }

         // update bisection endpoints
        if ( func < 0.0f )
        {
            a = p;
        }
        else
        {
            b = p;
        }

        // get speed along curve
        float speed = Velocity(p).Length();

        // if result will lie outside [a,b] 
        if ( ((p-a)*speed - func)*((p-b)*speed - func) > -1.0e-3f )
        {
            // do bisection
            p = 0.5f*(a+b);
        }    
        else
        {
            // otherwise Newton-Raphson
            p -= func/speed;
        }
    }

    // done iterating, return failure case
    return FLT_MAX;

}   // End of Hermite::FindParameterByDistance()


//-------------------------------------------------------------------------------
// @ Hermite::ArcLength()
//-------------------------------------------------------------------------------
// Find length of curve between parameters t1 and t2
//-------------------------------------------------------------------------------
float 
Hermite::ArcLength( float t1, float t2 )
{
    if ( t2 <= t1 )
        return 0.0f;

    if ( t1 < mTimes[0] )
        t1 = mTimes[0];

    if ( t2 > mTimes[mCount-1] )
        t2 = mTimes[mCount-1];

    // find segment and parameter
    unsigned int seg1;
    for ( seg1 = 0; seg1 < mCount-1; ++seg1 )
    {
        if ( t1 < mTimes[seg1+1] )
        {
            break;
        }
    }
    float u1 = (t1 - mTimes[seg1])/(mTimes[seg1+1] - mTimes[seg1]);
    
    // find segment and parameter
    unsigned int seg2;
    for ( seg2 = 0; seg2 < mCount-1; ++seg2 )
    {
        if ( t2 <= mTimes[seg2+1] )
        {
            break;
        }
    }
    float u2 = (t2 - mTimes[seg2])/(mTimes[seg2+1] - mTimes[seg2]);
    
    float result;
    // both parameters lie in one segment
    if ( seg1 == seg2 )
    {
        result = SegmentArcLength( seg1, u1, u2 );
    }
    // parameters cross segments
    else
    {
        result = SegmentArcLength( seg1, u1, 1.0f );
        for ( u_int i = seg1+1; i < seg2; ++i )
            result += mLengths[i];
        result += SegmentArcLength( seg2, 0.0f, u2 );
    }

    return result;

}   // End of Hermite::ArcLength()


//-------------------------------------------------------------------------------
// @ Hermite::SegmentArcLength()
//-------------------------------------------------------------------------------
// Find length of curve segment between parameters u1 and u2
//-------------------------------------------------------------------------------
float 
Hermite::SegmentArcLength( u_int i, float u1, float u2 )
{
    static const float x[] =
    {
        0.0000000000f, 0.5384693101f, -0.5384693101f, 0.9061798459f, -0.9061798459f 
    };

    static const float c[] =
    {
        0.5688888889f, 0.4786286705f, 0.4786286705f, 0.2369268850f, 0.2369268850f
    };

    //ASSERT(i >= 0 && i < mCount-1);

    if ( u2 <= u1 )
        return 0.0f;

    if ( u1 < 0.0f )
        u1 = 0.0f;

    if ( u2 > 1.0f )
        u2 = 1.0f;

    // use Gaussian quadrature
    float sum = 0.0f;
    // set up for computation of Hermite derivative
    Vector3 A = 2.0f*mPositions[i]
                - 2.0f*mPositions[i+1]
                + mInTangents[i]
                + mOutTangents[i];
    Vector3 B = -3.0f*mPositions[i]
                + 3.0f*mPositions[i+1]
                - 2.0f*mInTangents[i]
                - mOutTangents[i];
    Vector3 C = mInTangents[i];
    
    for ( u_int j = 0; j < 5; ++j )
    {
        float u = 0.5f*((u2 - u1)*x[j] + u2 + u1);
        Vector3 derivative = C + u*(2.0f*B + 3.0f*u*A);
        sum += c[j]*derivative.Length();
    }
    sum *= 0.5f*(u2-u1);

    return sum;

}   // End of Hermite::SegmentArcLength()


} // namespace gfx
