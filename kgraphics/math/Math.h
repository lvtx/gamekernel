#pragma once 

typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;

#include <math.h>

//-------------------------------------------------------------------------------
//-- Typedefs, Structs ----------------------------------------------------------
//-------------------------------------------------------------------------------

#define kEpsilon    1.0e-6f

#define kPI         3.1415926535897932384626433832795f
#define kHalfPI     1.5707963267948966192313216916398f
#define kTwoPI      2.0f*kPI

#ifdef PLATFORM_OSX
#define sqrtf sqrt
#define sinf sin
#define cosf cos
#define fabsf fabs
#define tanf tan
#define atan2f atan2
#define acosf acos
#endif



namespace gfx {

inline float Sqrt( float val )        { return sqrtf( val ); }

// XXX: Serious bug. Divide by 0 
inline float InvSqrt( float val )     { return 1.0f/sqrtf( val ); }
inline float Abs( float f )           { return fabsf(f); }

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Functions --------------------------------------------------------------------
//-------------------------------------------------------------------------------
extern void FastSinCos( float a, float& sina, float& cosa );

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ IsZero()
//-------------------------------------------------------------------------------
// Is this floating point value close to zero?
//-------------------------------------------------------------------------------
inline Bool IsZero( float a ) 
{
    return ( fabsf(a) < kEpsilon );

}   // End of IsZero()

//-------------------------------------------------------------------------------
// @ AreEqual()
//-------------------------------------------------------------------------------
// Are these floating point values close to equal?
//-------------------------------------------------------------------------------
inline Bool AreEqual( float a, float b ) 
{
    return ( IsZero(a-b) );

}   // End of AreEqual()


//-------------------------------------------------------------------------------
// @ Sin()
//-------------------------------------------------------------------------------
// Returns the floating-point sine of the argument
//-------------------------------------------------------------------------------
inline float Sin( float a )
{
    return sinf(a);

}  // End of Sin


//-------------------------------------------------------------------------------
// @ Cos()
//-------------------------------------------------------------------------------
// Returns the floating-point cosine of the argument
//-------------------------------------------------------------------------------
inline float Cos( float a )
{
    return cosf(a);

}  // End of Cos


//-------------------------------------------------------------------------------
// @ Tan()
//-------------------------------------------------------------------------------
// Returns the floating-point tangent of the argument
//-------------------------------------------------------------------------------
inline float Tan( float a )
{
    return tanf(a);

}  // End of Tan


//-------------------------------------------------------------------------------
// @ SinCos()
//-------------------------------------------------------------------------------
// Returns the floating-point sine and cosine of the argument
//-------------------------------------------------------------------------------
inline void SinCos( float a, float& sina, float& cosa )
{
    sina = sinf(a);
    cosa = cosf(a);

}  // End of SinCos

/**
 * 각도를 라디안으로 변경 
 */
inline float DegreeToRadian( float a )
{
	return ( a / 180.0f ) * kPI; 
}

inline float RadianToDegree( float r )
{
	return ( r / kPI ) * 180;
}

} // namespace gfx
