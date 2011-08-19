#include <stdafx.h>

#include <kgraphics/math/Bezier.h>
#include <kgraphics/math/math.h>
#include <kgraphics/math/LineSegment3.h>
#include <float.h>

namespace gfx { 

//-------------------------------------------------------------------------------
// @ Bezier::Bezier()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
Bezier::Bezier() :
    mPositions( 0 ),
    mControls( 0 ),
    mTimes( 0 ),
    mLengths( 0 ),
    mTotalLength( 0.0f ),
    mCount( 0 )
{
}   // End of Bezier::Bezier()


//-------------------------------------------------------------------------------
// @ Bezier::Bezier()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Bezier::~Bezier()
{
    Clean();

}   // End of Bezier::~Bezier()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const Bezier& source)
{
    out << source.mCount << eol;
    for (unsigned int i = 0; i < source.mCount; ++i )
    {
        out << source.mTimes[i] << ':' << source.mPositions[i] << eol;
        if ( i < source.mCount-1 )
            out << source.mControls[2*i] << ',' << source.mControls[2*i+1] << eol;
    }

    return out;
    
}   // End of operator<<()


//-------------------------------------------------------------------------------
// @ Bezier::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
bool
Bezier::Initialize( const Vector3* positions,
                       const Vector3* controls,
                       const float* times,
                       unsigned int count )

{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 2 || !positions || !times || !controls )
        return false;

    // set up arrays
    mPositions = new Vector3[count];
    mControls = new Vector3[2*(count-1)];
    mTimes = new float[count];
    mCount = count;

    // copy interpolant data
    unsigned int i;
    for ( i = 0; i < count; ++i )
    {
        mPositions[i] = positions[i];
        mTimes[i] = times[i];
    }

    // copy approximating control points
    for ( i = 0; i < 2*(count-1); ++i )
    {
        mControls[i] = controls[i];
    }

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( i = 0; i < count-1; ++i )
    {
        mLengths[i] = segmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    return true;

}   // End of Bezier::Initialize()


//-------------------------------------------------------------------------------
// @ Bezier::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
bool
Bezier::Initialize( const Vector3* positions,
                       const float* times,
                       unsigned int count )

{
    // make sure not already initialized
    if (mCount != 0)
        return false;

    // make sure data is valid
    if ( count < 2 || !positions || !times )
        return false;

    // set up arrays
    mPositions = new Vector3[count];
    mControls = new Vector3[2*(count-1)];
    mTimes = new float[count];
    mCount = count;

    // copy interpolant data
    unsigned int i;
    for ( i = 0; i < count; ++i )
    {
        mPositions[i] = positions[i];
        mTimes[i] = times[i];
    }

    // create approximating control points
    for ( i = 0; i < count-1; ++i )
    {
        if ( i > 0 )
            mControls[2*i] = mPositions[i] + (mPositions[i+1]-mPositions[i-1])/3.0f;
        if ( i < count-2 )
            mControls[2*i+1] = mPositions[i+1] - (mPositions[i+2]-mPositions[i])/3.0f;
    }
    mControls[0] = mControls[1] - (mPositions[1] - mPositions[0])/3.0f;
    mControls[2*count-3] = 
            mControls[2*count-4] + (mPositions[count-1] - mPositions[count-2])/3.0f;

    // set up curve segment lengths
    mLengths = new float[count-1];
    mTotalLength = 0.0f;
    for ( i = 0; i < count-1; ++i )
    {
        mLengths[i] = segmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    return true;

}   // End of Bezier::Initialize()


//-------------------------------------------------------------------------------
// @ Bezier::Clean()
//-------------------------------------------------------------------------------
// Clean out data
//-------------------------------------------------------------------------------
void Bezier::Clean()
{
    delete [] mPositions;
    delete [] mControls;
    delete [] mTimes;
    delete [] mLengths;
    mTotalLength = 0.0f;
    mCount = 0;

}   // End of Bezier::Clean()


//-------------------------------------------------------------------------------
// @ Bezier::Evaluate()
//-------------------------------------------------------------------------------
// Evaluate spline
//-------------------------------------------------------------------------------
Vector3
Bezier::Evaluate( float t )
{
    // make sure data is valid
    // //ASSERT( mCount >= 2 );

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
    Vector3 A = mPositions[i+1]
                - 3.0f*mControls[2*i+1]
                + 3.0f*mControls[2*i]
                - mPositions[i];
    Vector3 B = 3.0f*mControls[2*i+1]
                - 6.0f*mControls[2*i]
                + 3.0f*mPositions[i];
    Vector3 C = 3.0f*mControls[2*i]
                - 3.0f*mPositions[i];
    
    return mPositions[i] + u*(C + u*(B + u*A));

}   // End of Bezier::Evaluate()


//-------------------------------------------------------------------------------
// @ Bezier::Velocity()
//-------------------------------------------------------------------------------
// Evaluate spline's derivative
//-------------------------------------------------------------------------------
Vector3
Bezier::Velocity( float t )
{
    // make sure data is valid
    // //ASSERT( mCount >= 2 );

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
    Vector3 A = mPositions[i+1]
                - 3.0f*mControls[2*i+1]
                + 3.0f*mControls[2*i]
                - mPositions[i];
    Vector3 B = 6.0f*mControls[2*i+1]
                - 12.0f*mControls[2*i]
                + 6.0f*mPositions[i];
    Vector3 C = 3.0f*mControls[2*i]
                - 3.0f*mPositions[i];
    
    return C + u*(B + 3.0f*u*A);

}   // End of Bezier::Velocity()


//-------------------------------------------------------------------------------
// @ Bezier::Acceleration()
//-------------------------------------------------------------------------------
// Evaluate spline's second derivative
//-------------------------------------------------------------------------------
Vector3
Bezier::Acceleration( float t )
{
    // make sure data is valid
    // //ASSERT( mCount >= 2 );

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
    Vector3 A = mPositions[i+1]
                - 3.0f*mControls[2*i+1]
                + 3.0f*mControls[2*i]
                - mPositions[i];
    Vector3 B = 6.0f*mControls[2*i+1]
                - 12.0f*mControls[2*i]
                + 6.0f*mPositions[i];
    
    return B + 6.0f*u*A;

}   // End of Bezier::Acceleration()


//-------------------------------------------------------------------------------
// @ Bezier::FindParameterByDistance()
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
Bezier::FindParameterByDistance( float t1, float s )
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

}   // End of Bezier::FindParameterByDistance()


//-------------------------------------------------------------------------------
// @ Bezier::ArcLength()
//-------------------------------------------------------------------------------
// Find length of curve between parameters t1 and t2
//-------------------------------------------------------------------------------
float 
Bezier::ArcLength( float t1, float t2 )
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
        result = segmentArcLength( seg1, u1, u2 );
    }
    // parameters cross segments
    else
    {
        result = segmentArcLength( seg1, u1, 1.0f );
        for ( u_int i = seg1+1; i < seg2; ++i )
            result += mLengths[i];
        result += segmentArcLength( seg2, 0.0f, u2 );
    }

    return result;

}   // End of Bezier::ArcLength()


//-------------------------------------------------------------------------------
// @ Bezier::segmentArcLength()
//-------------------------------------------------------------------------------
// Find length of curve segment between parameters u1 and u2
//-------------------------------------------------------------------------------
float 
Bezier::segmentArcLength( u_int i, float u1, float u2 )
{
    // //ASSERT(i >= 0 && i < mCount-1);

    if ( u2 <= u1 )
        return 0.0f;

    if ( u1 < 0.0f )
        u1 = 0.0f;

    if ( u2 > 1.0f )
        u2 = 1.0f;

    Vector3 P0 = mPositions[i];
    Vector3 P1 = mControls[2*i];
    Vector3 P2 = mControls[2*i+1];
    Vector3 P3 = mPositions[i+1];

    // get control points for subcurve from 0.0 to u2 (de Casteljau's method)
    float minus_u2 = (1.0f - u2);
    Vector3 L1 = minus_u2*P0 + u2*P1;
    Vector3 H = minus_u2*P1 + u2*P2;
    Vector3 L2 = minus_u2*L1 + u2*H;
    Vector3 L3 = minus_u2*L2 + u2*(minus_u2*H + u2*(minus_u2*P2 + u2*P3));

    // resubdivide to get control points for subcurve between u1 and u2
    float minus_u1 = (1.0f - u1);
    H = minus_u1*L1 + u1*L2;
    Vector3 R3 = L3;
    Vector3 R2 = minus_u1*L2 + u1*L3;
    Vector3 R1 = minus_u1*H + u1*R2;
    Vector3 R0 = minus_u1*(minus_u1*(minus_u1*P0 + u1*L1) + u1*H) + u1*R1;

    // get length through subdivision
    return SubdivideLength( R0, R1, R2, R3 );

}   // End of Bezier::SegmentArcLength()


//-------------------------------------------------------------------------------
// @ Bezier::SubdivideLength()
//-------------------------------------------------------------------------------
// Get length of Bezier curve using midpoint subdivision
//-------------------------------------------------------------------------------
float 
Bezier::SubdivideLength( const Vector3& P0, const Vector3& P1, 
                     const Vector3& P2, const Vector3& P3 )
{
    // check to see if basically straight
    float Lmin = gfx::Distance( P0, P3 );
    float Lmax = gfx::Distance( P0, P1 ) + gfx::Distance( P1, P2 ) + gfx::Distance( P2, P3 );
    float diff = Lmin - Lmax;

    if ( diff*diff < 1.0e-3f )
        return 0.5f*(Lmin + Lmax);

    // otherwise get control points for subdivision
    Vector3 L1 = (P0 + P1) * 0.5f;
    Vector3 H = (P1 + P2) * 0.5f;
    Vector3 L2 = (L1 + H) * 0.5f;
    Vector3 R2 = (P2 + P3) * 0.5f;
    Vector3 R1 = (H + R2) * 0.5f;
    Vector3 mid = (L2 + R1) * 0.5f;

    // subdivide
    return SubdivideLength( P0, L1, L2, mid ) + SubdivideLength( mid, R1, R2, P3 );

}   // End of Bezier::SubdivideLength()

} // namespace gfx
