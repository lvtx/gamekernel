#include <stdafx.h>

#include <kgraphics/math/Quat.h>
#include <kgraphics/math/math.h>
#include <kgraphics/math/Vector3.h>
#include <kgraphics/math/Matrix33.h> 

namespace gfx {

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

Quat Quat::zero( 0.0f, 0.0f, 0.0f, 0.0f );
Quat Quat::identity( 1.0f, 0.0f, 0.0f, 0.0f );

//-------------------------------------------------------------------------------
// @ Quat::Quat()
//-------------------------------------------------------------------------------
// Axis-angle constructor
//-------------------------------------------------------------------------------
Quat::Quat( const Vector3& axis, float angle )
{
    Set( axis, angle );
}   // End of Quat::Quat()


//-------------------------------------------------------------------------------
// @ Quat::Quat()
//-------------------------------------------------------------------------------
// To-from vector constructor
//-------------------------------------------------------------------------------
Quat::Quat( const Vector3& from, const Vector3& to )
{
    Set( from, to );
}   // End of Quat::Quat()

//-------------------------------------------------------------------------------
// @ Quat::Quat()
//-------------------------------------------------------------------------------
// Vector constructor
//-------------------------------------------------------------------------------
Quat::Quat( const Vector3& vector )
{
    Set( 0.0f, vector.x, vector.y, vector.z );
}   // End of Quat::Quat()


//-------------------------------------------------------------------------------
// @ Quat::Quat()
//-------------------------------------------------------------------------------
// Rotation matrix constructor
//-------------------------------------------------------------------------------
Quat::Quat( const Matrix33& rotation )
{
    float trace = rotation.Trace();
    if ( trace > 0.0f )
    {
        float s = Sqrt( trace + 1.0f );
        w = s*0.5f;
        float recip = 0.5f/s;
        x = (rotation(2,1) - rotation(1,2))*recip;
        y = (rotation(0,2) - rotation(2,0))*recip;
        z = (rotation(1,0) - rotation(0,1))*recip;
    }
    else
    {
        unsigned int i = 0;
        if ( rotation(1,1) > rotation(0,0) )
            i = 1;
        if ( rotation(2,2) > rotation(i,i) )
            i = 2;
        unsigned int j = (i+1)%3;
        unsigned int k = (j+1)%3;
        float s = Sqrt( rotation(i,i) - rotation(j,j) - rotation(k,k) + 1.0f );
        (*this)[i] = 0.5f*s;
        float recip = 0.5f/s;
        w = (rotation(k,j) - rotation(j,k))*recip;
        (*this)[j] = (rotation(j,i) + rotation(i,j))*recip;
        (*this)[k] = (rotation(k,i) + rotation(i,k))*recip;
    }

}   // End of Quat::Quat()


//-------------------------------------------------------------------------------
// @ Quat::Quat()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
Quat::Quat(const Quat& other) : 
    w( other.w ),
    x( other.x ),
    y( other.y ),
    z( other.z )
{

}   // End of Quat::Quat()


//-------------------------------------------------------------------------------
// @ Quat::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
Quat&
Quat::operator=(const Quat& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    w = other.w;
    x = other.x;
    y = other.y;
    z = other.z;
    
    return *this;

}   // End of Quat::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const Quat& source)
{
    out << '[' << source.w << ',' << source.x << ',' 
        << source.y << ',' << source.z << ']';

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ Quat::Magnitude()
//-------------------------------------------------------------------------------
// Quaternion magnitude (square root of norm)
//-------------------------------------------------------------------------------
float 
Quat::Magnitude() const
{
    return Sqrt( w*w + x*x + y*y + z*z );

}   // End of Quat::Magnitude()


//-------------------------------------------------------------------------------
// @ Quat::Norm()
//-------------------------------------------------------------------------------
// Quaternion norm
//-------------------------------------------------------------------------------
float 
Quat::Norm() const
{
    return ( w*w + x*x + y*y + z*z );

}   // End of Quat::Norm()


//-------------------------------------------------------------------------------
// @ Quat::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
Quat::operator==( const Quat& other ) const
{
    if ( gfx::IsZero( other.w - w )
        && gfx::IsZero( other.x - x )
        && gfx::IsZero( other.y - y )
        && gfx::IsZero( other.z - z ) )
        return true;

    return false;   
}   // End of Quat::operator==()


//-------------------------------------------------------------------------------
// @ Quat::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
bool 
Quat::operator!=( const Quat& other ) const
{
    if ( gfx::IsZero( other.w - w )
        || gfx::IsZero( other.x - x )
        || gfx::IsZero( other.y - y )
        || gfx::IsZero( other.z - z ) )
        return false;

    return true;
}   // End of Quat::operator!=()


//-------------------------------------------------------------------------------
// @ Quat::IsZero()
//-------------------------------------------------------------------------------
// Check for zero quat
//-------------------------------------------------------------------------------
bool 
Quat::IsZero() const
{
    return gfx::IsZero(w*w + x*x + y*y + z*z);

}   // End of Quat::IsZero()


//-------------------------------------------------------------------------------
// @ Quat::IsUnit()
//-------------------------------------------------------------------------------
// Check for unit quat
//-------------------------------------------------------------------------------
bool 
Quat::IsUnit() const
{
    return gfx::IsZero(1.0f - w*w - x*x - y*y - z*z);

}   // End of Quat::IsUnit()


//-------------------------------------------------------------------------------
// @ Quat::IsIdentity()
//-------------------------------------------------------------------------------
// Check for identity quat
//-------------------------------------------------------------------------------
bool 
Quat::IsIdentity() const
{
    return gfx::IsZero(1.0f - w)
        && gfx::IsZero( x ) 
        && gfx::IsZero( y )
        && gfx::IsZero( z );

}   // End of Quat::IsIdentity()


//-------------------------------------------------------------------------------
// @ Quat::Set()
//-------------------------------------------------------------------------------
// Set quaternion based on axis-angle
//-------------------------------------------------------------------------------
void
Quat::Set( const Vector3& axis, float angle )
{
    // if axis of rotation is zero vector, just set to identity quat
    float length = axis.LengthSquared();
    if ( gfx::IsZero( length ) )
    {
        Identity();
        return;
    }

    // take half-angle
    angle *= 0.5f;

    float sintheta, costheta;
    SinCos(angle, sintheta, costheta);

    float scaleFactor = sintheta/Sqrt( length );

    w = costheta;
    x = scaleFactor * axis.x;
    y = scaleFactor * axis.y;
    z = scaleFactor * axis.z;

}   // End of Quat::Set()


//-------------------------------------------------------------------------------
// @ Quat::Set()
//-------------------------------------------------------------------------------
// Set quaternion based on start and end vectors
// 
// This is a slightly faster method than that presented in the book, and it 
// doesn't require unit vectors as input.  Found on GameDev.net, in an article by
// minorlogic.  Original source unknown.
//-------------------------------------------------------------------------------
void
Quat::Set( const Vector3& from, const Vector3& to )
{
   // get axis of rotation
    Vector3 axis = from.Cross( to );

    // get scaled cos of angle between vectors and set initial quaternion
    Set(  from.Dot( to ), axis.x, axis.y, axis.z );
    // quaternion at this point is ||from||*||to||*( cos(theta), r*sin(theta) )

    // normalize to remove ||from||*||to|| factor
    Normalize();
    // quaternion at this point is ( cos(theta), r*sin(theta) )
    // what we want is ( cos(theta/2), r*sin(theta/2) )

    // set up for half angle calculation
    w += 1.0f;

    // now when we normalize, we'll be dividing by sqrt(2*(1+cos(theta))), which is 
    // what we want for r*sin(theta) to give us r*sin(theta/2)  (see pages 487-488)
    // 
    // w will become 
    //                 1+cos(theta)
    //            ----------------------
    //            sqrt(2*(1+cos(theta)))        
    // which simplifies to
    //                cos(theta/2)

    // before we normalize, check if vectors are opposing
    if ( w <= kEpsilon )
    {
        // rotate pi radians around orthogonal vector
        // take cross product with x axis
        if ( from.z*from.z > from.x*from.x )
            Set( 0.0f, 0.0f, from.z, -from.y );
        // or take cross product with z axis
        else
            Set( 0.0f, from.y, -from.x, 0.0f );
    }
   
    // normalize again to get rotation quaternion
    Normalize();

}   // End of Quat::Set()


//-------------------------------------------------------------------------------
// @ Quat::Set()
//-------------------------------------------------------------------------------
// Set quaternion based on fixed angles
//-------------------------------------------------------------------------------
void 
Quat::Set( float zRotation, float yRotation, float xRotation ) 
{
    zRotation *= 0.5f;
    yRotation *= 0.5f;
    xRotation *= 0.5f;

    // get sines and cosines of half angles
    float Cx, Sx;
    SinCos(xRotation, Sx, Cx);

    float Cy, Sy;
    SinCos(yRotation, Sy, Cy);

    float Cz, Sz;
    SinCos(zRotation, Sz, Cz);

    // multiply it out
    w = Cx*Cy*Cz - Sx*Sy*Sz;
    x = Sx*Cy*Cz + Cx*Sy*Sz;
    y = Cx*Sy*Cz - Sx*Cy*Sz;
    z = Cx*Cy*Sz + Sx*Sy*Cx;

}   // End of Quat::Set()


//-------------------------------------------------------------------------------
// @ Quat::GetAxisAngle()
//-------------------------------------------------------------------------------
// Get axis-angle based on quaternion
//-------------------------------------------------------------------------------
void
Quat::GetAxisAngle( Vector3& axis, float& angle )
{
    angle = 2.0f*acosf( w );
    float length = Sqrt( 1.0f - w*w );
    if ( gfx::IsZero(length) )
        axis.Zero();
    else
    {
        length = 1.0f/length;
        axis.Set( x*length, y*length, z*length );
    }

}   // End of Quat::GetAxisAngle()


//-------------------------------------------------------------------------------
// @ Quat::Clean()
//-------------------------------------------------------------------------------
// Set elements close to zero equal to zero
//-------------------------------------------------------------------------------
void
Quat::Clean()
{
    if ( gfx::IsZero( w ) )
        w = 0.0f;
    if ( gfx::IsZero( x ) )
        x = 0.0f;
    if ( gfx::IsZero( y ) )
        y = 0.0f;
    if ( gfx::IsZero( z ) )
        z = 0.0f;

}   // End of Quat::Clean()


//-------------------------------------------------------------------------------
// @ Quat::Normalize()
//-------------------------------------------------------------------------------
// Set to unit quaternion
//-------------------------------------------------------------------------------
void
Quat::Normalize()
{
    float lengthsq = w*w + x*x + y*y + z*z;

    if ( gfx::IsZero( lengthsq ) )
    {
        Zero();
    }
    else
    {
        float factor = InvSqrt( lengthsq );
        w *= factor;
        x *= factor;
        y *= factor;
        z *= factor;
    }

}   // End of Quat::Normalize()


//-------------------------------------------------------------------------------
// @ ::Conjugate()
//-------------------------------------------------------------------------------
// Compute complex conjugate
//-------------------------------------------------------------------------------
Quat 
Conjugate( const Quat& quat ) 
{
    return Quat( quat.w, -quat.x, -quat.y, -quat.z );

}   // End of Conjugate()


//-------------------------------------------------------------------------------
// @ Quat::Conjugate()
//-------------------------------------------------------------------------------
// Set self to complex conjugate
//-------------------------------------------------------------------------------
const Quat& 
Quat::Conjugate()
{
    x = -x;
    y = -y;
    z = -z;

    return *this;

}   // End of Conjugate()


//-------------------------------------------------------------------------------
// @ ::Inverse()
//-------------------------------------------------------------------------------
// Compute quaternion inverse
//-------------------------------------------------------------------------------
Quat 
Inverse( const Quat& quat )
{
    float norm = quat.w*quat.w + quat.x*quat.x + quat.y*quat.y + quat.z*quat.z;
    // if we're the zero quaternion, just return identity
    if ( !gfx::IsZero( norm ) )
    {
        // //ASSERT( false );
        return Quat();
    }

    float normRecip = 1.0f / norm;
    return Quat( normRecip*quat.w, -normRecip*quat.x, -normRecip*quat.y, 
                   -normRecip*quat.z );

}   // End of Inverse()


//-------------------------------------------------------------------------------
// @ Quat::Inverse()
//-------------------------------------------------------------------------------
// Set self to inverse
//-------------------------------------------------------------------------------
const Quat& 
Quat::Inverse()
{
    float norm = w*w + x*x + y*y + z*z;
    // if we're the zero quaternion, just return
    if ( gfx::IsZero( norm ) )
        return *this;

    float normRecip = 1.0f / norm;
    w = normRecip*w;
    x = -normRecip*x;
    y = -normRecip*y;
    z = -normRecip*z;

    return *this;

}   // End of Inverse()


//-------------------------------------------------------------------------------
// @ Quat::operator+()
//-------------------------------------------------------------------------------
// Add quat to self and return
//-------------------------------------------------------------------------------
Quat 
Quat::operator+( const Quat& other ) const
{
    return Quat( w + other.w, x + other.x, y + other.y, z + other.z );

}   // End of Quat::operator+()


//-------------------------------------------------------------------------------
// @ Quat::operator+=()
//-------------------------------------------------------------------------------
// Add quat to self, store in self
//-------------------------------------------------------------------------------
Quat& 
Quat::operator+=( const Quat& other )
{
    w += other.w;
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;

}   // End of Quat::operator+=()


//-------------------------------------------------------------------------------
// @ Quat::operator-()
//-------------------------------------------------------------------------------
// Subtract quat from self and return
//-------------------------------------------------------------------------------
Quat 
Quat::operator-( const Quat& other ) const
{
    return Quat( w - other.w, x - other.x, y - other.y, z - other.z );

}   // End of Quat::operator-()


//-------------------------------------------------------------------------------
// @ Quat::operator-=()
//-------------------------------------------------------------------------------
// Subtract quat from self, store in self
//-------------------------------------------------------------------------------
Quat& 
Quat::operator-=( const Quat& other )
{
    w -= other.w;
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;

}   // End of Quat::operator-=()


//-------------------------------------------------------------------------------
// @ Quat::operator-=() (unary)
//-------------------------------------------------------------------------------
// Negate self and return
//-------------------------------------------------------------------------------
Quat
Quat::operator-() const
{
    return Quat(-w, -x, -y, -z);
}    // End of Quat::operator-()


//-------------------------------------------------------------------------------
// @ operator*()
//-------------------------------------------------------------------------------
// Scalar multiplication
//-------------------------------------------------------------------------------
Quat  
operator*( float scalar, const Quat& quat )
{
    return Quat( scalar*quat.w, scalar*quat.x, scalar*quat.y, scalar*quat.z );

}   // End of operator*()


//-------------------------------------------------------------------------------
// @ Quat::operator*=()
//-------------------------------------------------------------------------------
// Scalar multiplication by self
//-------------------------------------------------------------------------------
Quat&
Quat::operator*=( float scalar )
{
    w *= scalar;
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;

}   // End of Quat::operator*=()


//-------------------------------------------------------------------------------
// @ Quat::operator*()
//-------------------------------------------------------------------------------
// Quaternion multiplication
//-------------------------------------------------------------------------------
Quat  
Quat::operator*( const Quat& other ) const
{
    return Quat( w*other.w - x*other.x - y*other.y - z*other.z,
                   w*other.x + x*other.w + y*other.z - z*other.y,
                   w*other.y + y*other.w + z*other.x - x*other.z,
                   w*other.z + z*other.w + x*other.y - y*other.x );

}   // End of Quat::operator*()


//-------------------------------------------------------------------------------
// @ Quat::operator*=()
//-------------------------------------------------------------------------------
// Quaternion multiplication by self
//-------------------------------------------------------------------------------
Quat&
Quat::operator*=( const Quat& other )
{
    Set( w*other.w - x*other.x - y*other.y - z*other.z,
         w*other.x + x*other.w + y*other.z - z*other.y,
         w*other.y + y*other.w + z*other.x - x*other.z,
         w*other.z + z*other.w + x*other.y - y*other.x );
  
    return *this;

}   // End of Quat::operator*=()


//-------------------------------------------------------------------------------
// @ Quat::Dot()
//-------------------------------------------------------------------------------
// Dot product by self
//-------------------------------------------------------------------------------
float               
Quat::Dot( const Quat& quat ) const
{
    return ( w*quat.w + x*quat.x + y*quat.y + z*quat.z);

}   // End of Quat::Dot()


//-------------------------------------------------------------------------------
// @ Dot()
//-------------------------------------------------------------------------------
// Dot product friend operator
//-------------------------------------------------------------------------------
float               
Dot( const Quat& quat1, const Quat& quat2 )
{
    return (quat1.w*quat2.w + quat1.x*quat2.x + quat1.y*quat2.y + quat1.z*quat2.z);

}   // End of Dot()


//-------------------------------------------------------------------------------
// @ Quat::Rotate()
//-------------------------------------------------------------------------------
// Rotate vector by quaternion
// Assumes quaternion is normalized!
//-------------------------------------------------------------------------------
Vector3   
Quat::Rotate( const Vector3& vector ) const
{
    // //ASSERT( IsUnit() );

    float vMult = 2.0f*(x*vector.x + y*vector.y + z*vector.z);
    float crossMult = 2.0f*w;
    float pMult = crossMult*w - 1.0f;

    return Vector3( pMult*vector.x + vMult*x + crossMult*(y*vector.z - z*vector.y),
                      pMult*vector.y + vMult*y + crossMult*(z*vector.x - x*vector.z),
                      pMult*vector.z + vMult*z + crossMult*(x*vector.y - y*vector.x) );

}   // End of Quat::Rotate()


//-------------------------------------------------------------------------------
// @ Lerp()
//-------------------------------------------------------------------------------
// Linearly interpolate two quaternions
// This will always take the shorter path between them
//-------------------------------------------------------------------------------
void 
Lerp( Quat& result, const Quat& start, const Quat& end, float t )
{
    // get cos of "angle" between quaternions
    float cosTheta = start.Dot( end );

    // initialize result
    result = t*end;

    // if "angle" between quaternions is less than 90 degrees
    if ( cosTheta >= kEpsilon )
    {
        // use standard interpolation
        result += (1.0f-t)*start;
    }
    else
    {
        // otherwise, take the shorter path
        result += (t-1.0f)*start;
    }

}   // End of Lerp()


//-------------------------------------------------------------------------------
// @ Slerp()
//-------------------------------------------------------------------------------
// Spherical linearly interpolate two quaternions
// This will always take the shorter path between them
//-------------------------------------------------------------------------------
void 
Slerp( Quat& result, const Quat& start, const Quat& end, float t )
{
    // get cosine of "angle" between quaternions
    float cosTheta = start.Dot( end );
    float startInterp, endInterp;

    // if "angle" between quaternions is less than 90 degrees
    if ( cosTheta >= kEpsilon )
    {
        // if angle is greater than zero
        if ( (1.0f - cosTheta) > kEpsilon )
        {
            // use standard slerp
            float theta = acosf( cosTheta );
            float recipSinTheta = 1.0f/Sin( theta );

            startInterp = Sin( (1.0f - t)*theta )*recipSinTheta;
            endInterp = Sin( t*theta )*recipSinTheta;
        }
        // angle is close to zero
        else
        {
            // use linear interpolation
            startInterp = 1.0f - t;
            endInterp = t;
        }
    }
    // otherwise, take the shorter route
    else
    {
        // if angle is less than 180 degrees
        if ( (1.0f + cosTheta) > kEpsilon )
        {
            // use slerp w/negation of start quaternion
            float theta = acosf( -cosTheta );
            float recipSinTheta = 1.0f/Sin( theta );

            startInterp = Sin( (t-1.0f)*theta )*recipSinTheta;
            endInterp = Sin( t*theta )*recipSinTheta;
        }
        // angle is close to 180 degrees
        else
        {
            // use lerp w/negation of start quaternion
            startInterp = t - 1.0f;
            endInterp = t;
        }
    }
    
    result = startInterp*start + endInterp*end;

}   // End of Slerp()


//-------------------------------------------------------------------------------
// @ ApproxSlerp()
//-------------------------------------------------------------------------------
// Approximate spherical linear interpolation of two quaternions
// Based on "Hacking Quaternions", Jonathan Blow, Game Developer, March 2002.
// See Game Developer, February 2004 for an alternate method.
//-------------------------------------------------------------------------------
void 
ApproxSlerp( Quat& result, const Quat& start, const Quat& end, float t )
{
    float cosTheta = start.Dot( end );

    // correct time by using cosine of angle between quaternions
    float factor = 1.0f - 0.7878088f*cosTheta;
    float k = 0.5069269f;
    factor *= factor;
    k *= factor;

    float b = 2*k;
    float c = -3*k;
    float d = 1 + k;

    t = t*(b*t + c) + d;

    // initialize result
    result = t*end;

    // if "angle" between quaternions is less than 90 degrees
    if ( cosTheta >= kEpsilon )
    {
        // use standard interpolation
        result += (1.0f-t)*start;
    }
    else
    {
        // otherwise, take the shorter path
        result += (t-1.0f)*start;
    }

}   // End of ApproxSlerp()

} // namespace gfx
