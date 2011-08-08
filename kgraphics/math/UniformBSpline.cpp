#include <stdafx.h>

#include <kgraphics/math/UniformBSpline.h>
#include <kgraphics/math/Bezier.h>
#include <kgraphics/math/math.h>
#include <float.h>

namespace gfx {

//-------------------------------------------------------------------------------
// @ UniformBSpline::UniformBSpline()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
UniformBSpline::UniformBSpline() :
    mPositions( 0 ),
    mTimes( 0 ),
    mCount( 0 )
{
}   // End of UniformBSpline::UniformBSpline()


//-------------------------------------------------------------------------------
// @ UniformBSpline::UniformBSpline()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
UniformBSpline::~UniformBSpline()
{
    Clean();

}   // End of UniformBSpline::~UniformBSpline()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const UniformBSpline& source)
{
    out << source.mCount << eol;
    for (unsigned int i = 0; i < source.mCount; ++i )
    {
        out << source.mTimes[i] << ':' << source.mPositions[i] << eol;
    }

    return out;
    
}   // End of operator<<()


//-------------------------------------------------------------------------------
// @ UniformBSpline::Initialize()
//-------------------------------------------------------------------------------
// Set up sample points
//-------------------------------------------------------------------------------
Bool
UniformBSpline::Initialize( const Vector3* positions,
                              unsigned int count,
                              float startTime, float endTime )

{
    // make sure not already initialized
    if ( mCount != 0 )
        return false;

    // make sure data is valid
    if ( count < 2 || !positions )
        return false;

    // set up arrays
    mPositions = new Vector3[count+4];
    mTimes = new float[count+2];
    mCount = count+4;

    // copy position data
    // triplicate start and end points so that curve passes through them
    mPositions[0] = mPositions[1] = positions[0];
    unsigned int i;
    for ( i = 0; i < count; ++i )
    {
        mPositions[i+2] = positions[i];
    }
    mPositions[mCount-1] = mPositions[mCount-2] = positions[count-1];

    // now set up times
    // we subdivide interval to get arrival times at each knot location
    float dt = (endTime - startTime)/(float)(count+1);
    mTimes[0] = startTime;
    for ( i = 0; i < count; ++i )
    {
        mTimes[i+1] = mTimes[i]+dt;
    }
    mTimes[count+1] = endTime;

    // set up curve segment lengths
    mLengths = new float[mCount-3];
    mTotalLength = 0.0f;
    for ( i = 0; i < mCount-3; ++i )
    {
        mLengths[i] = SegmentArcLength(i, 0.0f, 1.0f);
        mTotalLength += mLengths[i];
    }

    return true;

}   // End of UniformBSpline::Initialize()


//-------------------------------------------------------------------------------
// @ UniformBSpline::Clean()
//-------------------------------------------------------------------------------
// Clean out data
//-------------------------------------------------------------------------------
void
UniformBSpline::Clean()
{
    delete [] mPositions;
    delete [] mTimes;
    delete [] mLengths;
    mTotalLength = 0.0f;
    mCount = 0;

}   // End of UniformBSpline::~Clean()


//-------------------------------------------------------------------------------
// @ UniformBSpline::Evaluate()
//-------------------------------------------------------------------------------
// Evaluate spline
//-------------------------------------------------------------------------------
Vector3
UniformBSpline::Evaluate( float t )
{
    // make sure data is valid
    //ASSERT( mCount >= 6 );
    if ( mCount < 6 )
        return Vector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        return mPositions[0];
    else if ( t >= mTimes[mCount-3] )
        return mPositions[mCount-3];

    // find segment and parameter
    unsigned int i;  // segment #
    for ( i = 0; i < mCount-3; ++i )
    {
        if ( t <= mTimes[i+1] )
        {
            break;
        }
    }
    //ASSERT( i >= 0 && i < mCount-3 );

    float t0 = mTimes[i];
    float t1 = mTimes[i+1];
    float u = (t - t0)/(t1 - t0);

    // match segment index to standard B-spline terminology
    i += 3;

    // evaluate
    Vector3 A = mPositions[i]
                - 3.0f*mPositions[i-1]
                + 3.0f*mPositions[i-2]
                - mPositions[i-3];
    Vector3 B = 3.0f*mPositions[i-1]
                - 6.0f*mPositions[i-2]
                + 3.0f*mPositions[i-3];
    Vector3 C = 3.0f*mPositions[i-1] - 3.0f*mPositions[i-3];
    Vector3 D = mPositions[i-1]
                + 4.0f*mPositions[i-2]
                + mPositions[i-3];

    return (D + u*(C + u*(B + u*A)))/6.0f;

}   // End of UniformBSpline::Evaluate()


//-------------------------------------------------------------------------------
// @ UniformBSpline::Velocity()
//-------------------------------------------------------------------------------
// Evaluate derivative at parameter t
//-------------------------------------------------------------------------------
Vector3
UniformBSpline::Velocity( float t )
{
    // make sure data is valid
    //ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return Vector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        t = mTimes[0];
    else if ( t >= mTimes[mCount-3] )
        t = mTimes[mCount-3];

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

    // match segment index to standard B-spline terminology
    i += 3;

    // evaluate
    Vector3 A = mPositions[i]
                - 3.0f*mPositions[i-1]
                + 3.0f*mPositions[i-2]
                - mPositions[i-3];
    Vector3 B = 3.0f*mPositions[i-1]
                - 6.0f*mPositions[i-2]
                + 3.0f*mPositions[i-3];
    Vector3 C = 3.0f*mPositions[i-1] - 3.0f*mPositions[i-3];
    
    Vector3 result = (C + u*(2.0f*B + 3.0f*u*A))/6.0f;

    return result;

}   // End of UniformBSpline::Velocity()


//-------------------------------------------------------------------------------
// @ UniformBSpline::Acceleration()
//-------------------------------------------------------------------------------
// Evaluate second derivative at parameter t
//-------------------------------------------------------------------------------
Vector3
UniformBSpline::Acceleration( float t )
{
    // make sure data is valid
    //ASSERT( mCount >= 2 );
    if ( mCount < 2 )
        return Vector3::origin;

    // handle boundary conditions
    if ( t <= mTimes[0] )
        t = 0.0f;
    else if ( t > mTimes[mCount-3] )
        t = mTimes[mCount-3];

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

    // match segment index to standard B-spline terminology
    i += 3;

    // evaluate
    Vector3 A = mPositions[i]
                - 3.0f*mPositions[i-1]
                + 3.0f*mPositions[i-2]
                - mPositions[i-3];
    Vector3 B = 3.0f*mPositions[i-1]
                - 6.0f*mPositions[i-2]
                + 3.0f*mPositions[i-3];
    
    return 1.0f/3.0f*B + u*A;

}   // End of UniformBSpline::Acceleration()


//-------------------------------------------------------------------------------
// @ UniformBSpline::FindParameterByDistance()
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
UniformBSpline::FindParameterByDistance( float t1, float s )
{
    // initialize bisection endpoints
    float a = t1;
    float b = mTimes[mCount-3];

    // ensure that we remain within valid parameter space
    if ( s >= ArcLength(t1, b) )
        return b;
    if ( s <= 0.0f )
        return a;

    // make first guess
    float p = t1 + s*(mTimes[mCount-3]-mTimes[0])/mTotalLength;

    // iterate and look for zeros
    for ( u_int i = 0; i < 32; ++i )
    {
        // compute function value and test against zero
        float func = ArcLength(t1, p) - s;
        if ( Abs(func) < 1.0e-03f )
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

}   // End of UniformBSpline::FindParameterByDistance()


//-------------------------------------------------------------------------------
// @ UniformBSpline::ArcLength()
//-------------------------------------------------------------------------------
// Find length of curve between parameters t1 and t2
//-------------------------------------------------------------------------------
float 
UniformBSpline::ArcLength( float t1, float t2 )
{
    if ( t2 <= t1 )
        return 0.0f;

    if ( t1 < mTimes[0] )
        t1 = mTimes[0];

    if ( t2 > mTimes[mCount-3] )
        t2 = mTimes[mCount-3];

    // find segment and parameter
    unsigned int seg1;
    for ( seg1 = 0; seg1 < mCount-1; ++seg1 )
    {
        if ( t1 <= mTimes[seg1+1] )
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

}   // End of UniformBSpline::ArcLength()


//-------------------------------------------------------------------------------
// @ UniformBSpline::SegmentArcLength()
//-------------------------------------------------------------------------------
// Find length of curve segment between parameters u1 and u2
//-------------------------------------------------------------------------------
float 
UniformBSpline::SegmentArcLength( u_int i, float u1, float u2 )
{
    static const float x[] =
    {
        0.0000000000f, 0.5384693101f, -0.5384693101f, 0.9061798459f, -0.9061798459f 
    };

    static const float c[] =
    {
        0.5688888889f, 0.4786286705f, 0.4786286705f, 0.2369268850f, 0.2369268850f
    };

    //ASSERT(i >= 0 && i < mCount-3);

    if ( u2 <= u1 )
        return 0.0f;

    if ( u1 < 0.0f )
        u1 = 0.0f;

    if ( u2 > 1.0f )
        u2 = 1.0f;

    // reindex to use standard B-spline segment count
    i += 3;

    // use Gaussian quadrature
    float sum = 0.0f;
    // set up for computation of UniformBSpline derivative
    Vector3 A = mPositions[i]
                - 3.0f*mPositions[i-1]
                + 3.0f*mPositions[i-2]
                - mPositions[i-3];
    Vector3 B = 3.0f*mPositions[i-1]
                - 6.0f*mPositions[i-2]
                + 3.0f*mPositions[i-3];
    Vector3 C = 3.0f*mPositions[i-1] - 3.0f*mPositions[i-3];
    
    for ( u_int j = 0; j < 5; ++j )
    {
        float u = 0.5f*((u2 - u1)*x[j] + u2 + u1);
        Vector3 derivative = (C + u*(2.0f*B + 3.0f*u*A))/6.0f;
        sum += c[j]*derivative.Length();
    }
    sum *= 0.5f*(u2-u1);

    return sum;

}   // End of UniformBSpline::SegmentArcLength()

} // namespace gfx
