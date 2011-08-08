#include <stdafx.h>

#include "kgraphics/math/Covariance.h"
#include <kgraphics/math/Line3.h>
#include <kgraphics/math/LineSegment3.h>
#include <kgraphics/math/math.h>
#include <kgraphics/math/Matrix33.h>
#include "kgraphics/math/OBB.h"
#include <kgraphics/math/Plane.h>
#include <kgraphics/math/Quat.h>
#include <kgraphics/math/Ray3.h>
#include <kgraphics/math/Vector3.h>

namespace gfx {

//-------------------------------------------------------------------------------
// @ OBB::OBB()
//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
OBB::OBB(const OBB& other) : 
    mCenter( other.mCenter ),
    mRotation( other.mRotation ),
    mExtents( other.mExtents )
{

}   // End of OBB::OBB()


//-------------------------------------------------------------------------------
// @ OBB::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
OBB&
OBB::operator=(const OBB& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    mCenter = other.mCenter;
    mRotation = other.mRotation;
    mExtents = other.mExtents;
    
    return *this;

}   // End of OBB::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const OBB& source)
{
    out << source.mCenter;
    out << ' ' << source.mExtents;
    out << ' ' << source.mRotation;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ OBB::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
Bool 
OBB::operator==( const OBB& other ) const
{
    if ( other.mCenter == mCenter && other.mRotation == mRotation
         && other.mExtents == mExtents )
        return true;

    return false;   
}   // End of OBB::operator==()


//-------------------------------------------------------------------------------
// @ OBB::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
Bool 
OBB::operator!=( const OBB& other ) const
{
    if ( other.mCenter != mCenter || other.mRotation != mRotation
         || other.mExtents != mExtents )
        return true;

    return false;

}   // End of OBB::operator!=()


//-------------------------------------------------------------------------------
// @ OBB::Set()
//-------------------------------------------------------------------------------
// Set OBB based on set of points
//-------------------------------------------------------------------------------
void
OBB::Set( const Vector3* points, unsigned int nPoints )
{
    // //ASSERT( points );

    Vector3 centroid;

    // compute covariance matrix
    Matrix33 C;
    ComputeCovarianceMatrix( C, centroid, points, nPoints );

    // get basis vectors
    Vector3 basis[3];
    GetRealSymmetricEigenvectors( basis[0], basis[1], basis[2], C );
    mRotation.SetColumns( basis[0], basis[1], basis[2] );

    Vector3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    Vector3 max(FLT_MIN, FLT_MIN, FLT_MIN);

    // compute min, max projections of box on axes
    // for each point do 
    unsigned int i;
    for ( i = 0; i < nPoints; ++i )
    {
        Vector3 diff = points[i]-centroid;
        for (int j = 0; j < 3; ++j)
        {
            float length = diff.Dot(basis[j]);
            if (length > max[j])
            {
                max[j] = length;
            }
            else if (length < min[j])
            {
                min[j] = length;
            }
        }
    }

    // compute center, extents
    mCenter = centroid;
    for ( i = 0; i < 3; ++i )
    {
        mCenter += 0.5f*(min[i]+max[i])*basis[i];
        mExtents[i] = 0.5f*(max[i]-min[i]);
    }

}   // End of OBB::Set()


//----------------------------------------------------------------------------
// @ OBB::Transform()
// ---------------------------------------------------------------------------
// Transforms OBB into new space
//-----------------------------------------------------------------------------
OBB    
OBB::Transform( float scale, const Quat& rotate, 
    const Vector3& translate ) const
{
    OBB result;
    Matrix33 rotationMatrix(rotate);

    result.SetExtents( scale*mExtents );
    result.SetCenter( rotationMatrix*mCenter + translate );
    result.SetRotation( rotationMatrix*mRotation );

    return result;

}   // End of OBB::Transform()


//----------------------------------------------------------------------------
// @ OBB::Transform()
// ---------------------------------------------------------------------------
// Transforms OBB into new space
//-----------------------------------------------------------------------------
OBB    
OBB::Transform( float scale, const Matrix33& rotate, 
    const Vector3& translate ) const
{
    OBB result;

    result.SetExtents( scale*mExtents );
    result.SetCenter( rotate*mCenter + translate );
    result.SetRotation( rotate*mRotation );

    return result;
    
}   // End of OBB::Transform()


//----------------------------------------------------------------------------
// @ OBB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between OBB and OBB
//
// The efficiency of this could be improved slightly by computing factors only
// as we need them.  For example, we could compute only the first row of R
// before the first axis test, then the second row, etc.  It has been left this
// way because it's clearer.
//-----------------------------------------------------------------------------
Bool 
OBB::Intersect( const OBB& other ) const
{
    // extent vectors
    const Vector3& a = mExtents;    
    const Vector3& b = other.mExtents;

    // test factors
    float cTest, aTest, bTest;
    Bool parallelAxes = false;

    // transpose of rotation of B relative to A, i.e. (R_b^T * R_a)^T
    Matrix33 Rt = Transpose(mRotation)*other.mRotation;

    // absolute value of relative rotation matrix
    Matrix33 Rabs;  
    for ( unsigned int i = 0; i < 3; ++i )
    {
        for ( unsigned int j = 0; j < 3; ++j )
        {
			Rabs(i,j) = gfx::Abs( Rt(i,j ) );
            // if magnitude of dot product between axes is close to one
            if ( Rabs(i,j) + kEpsilon >= 1.0f )
            {
                // then box A and box B have near-parallel axes
                parallelAxes = true;
            }
        }
    }
    
    // relative translation (in A's frame)
    Vector3 c = (other.mCenter - mCenter)*mRotation;

    // separating axis A0
    cTest = Abs(c.x);
    aTest = a.x;
    bTest = b.x*Rabs(0,0)+b.y*Rabs(0,1)+b.z*Rabs(0,2);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1
    cTest = Abs(c.y);
    aTest = a.y;
    bTest = b.x*Rabs(1,0)+b.y*Rabs(1,1)+b.z*Rabs(1,2);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2
    cTest = Abs(c.z);
    aTest = a.z;
    bTest = b.x*Rabs(2,0)+b.y*Rabs(2,1)+b.z*Rabs(2,2);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis B0
    cTest = Abs( c.x*Rt(0,0) + c.y*Rt(1,0) + c.z*Rt(2,0) );
    aTest = a.x*Rabs(0,0)+a.y*Rabs(1,0)+a.z*Rabs(2,0);
    bTest = b.x;
    if ( cTest > aTest + bTest )
        return false;

    // separating axis B1
    cTest = Abs( c.x*Rt(0,1) + c.y*Rt(1,1) + c.z*Rt(2,1) );
    aTest = a.x*Rabs(0,1)+a.y*Rabs(1,1)+a.z*Rabs(2,1);
    bTest = b.y;
    if ( cTest > aTest + bTest )
        return false;

    // separating axis B2
    cTest = Abs( c.x*Rt(0,2) + c.y*Rt(1,2) + c.z*Rt(2,2) );
    aTest = a.x*Rabs(0,2)+a.y*Rabs(1,2)+a.z*Rabs(2,2);
    bTest = b.z;
    if ( cTest > aTest + bTest )
        return false;

    // if the two boxes have parallel axes, we're done, intersection
    if ( parallelAxes )
        return true;

    // separating axis A0 x B0
    cTest = Abs(c.z*Rt(1,0)-c.y*Rt(2,0));
    aTest = a.y*Rabs(2,0) + a.z*Rabs(1,0);
    bTest = b.y*Rabs(0,2) + b.z*Rabs(0,1);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A0 x B1
    cTest = Abs(c.z*Rt(1,1)-c.y*Rt(2,1));
    aTest = a.y*Rabs(2,1) + a.z*Rabs(1,1);
    bTest = b.x*Rabs(0,2) + b.z*Rabs(0,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A0 x B2
    cTest = Abs(c.z*Rt(1,2)-c.y*Rt(2,2));
    aTest = a.y*Rabs(2,2) + a.z*Rabs(1,2);
    bTest = b.x*Rabs(0,1) + b.y*Rabs(0,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1 x B0
    cTest = Abs(c.x*Rt(2,0)-c.z*Rt(0,0));
    aTest = a.x*Rabs(2,0) + a.z*Rabs(0,0);
    bTest = b.y*Rabs(1,2) + b.z*Rabs(1,1);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1 x B1
    cTest = Abs(c.x*Rt(2,1)-c.z*Rt(0,1));
    aTest = a.x*Rabs(2,1) + a.z*Rabs(0,1);
    bTest = b.x*Rabs(1,2) + b.z*Rabs(1,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A1 x B2
    cTest = Abs(c.x*Rt(2,2)-c.z*Rt(0,2));
    aTest = a.x*Rabs(2,2) + a.z*Rabs(0,2);
    bTest = b.x*Rabs(1,1) + b.y*Rabs(1,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2 x B0
    cTest = Abs(c.y*Rt(0,0)-c.x*Rt(1,0));
    aTest = a.x*Rabs(1,0) + a.y*Rabs(0,0);
    bTest = b.y*Rabs(2,2) + b.z*Rabs(2,1);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2 x B1
    cTest = Abs(c.y*Rt(0,1)-c.x*Rt(1,1));
    aTest = a.x*Rabs(1,1) + a.y*Rabs(0,1);
    bTest = b.x*Rabs(2,2) + b.z*Rabs(2,0);
    if ( cTest > aTest + bTest )
        return false;

    // separating axis A2 x B2
    cTest = Abs(c.y*Rt(0,2)-c.x*Rt(1,2));
    aTest = a.x*Rabs(1,2) + a.y*Rabs(0,2);
    bTest = b.x*Rabs(2,1) + b.y*Rabs(2,0);
    if ( cTest > aTest + bTest )
        return false;

    // all tests failed, have intersection
    return true;
}


//----------------------------------------------------------------------------
// @ OBB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between OBB and line
//-----------------------------------------------------------------------------
Bool
OBB::Intersect( const Line3& line ) const
{
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // compute difference vector
    Vector3 diff = mCenter - line.GetOrigin();

    // for each axis do
    for (int i = 0; i < 3; ++i)
    {
        // get axis i
        Vector3 axis = mRotation.GetColumn( i );

        // project relative vector onto axis
        float e = axis.Dot( diff );
        float f = line.GetDirection().Dot( axis );

        // ray is parallel to plane
        if ( gfx::IsZero( f ) )
        {
            // ray passes by box
            if ( -e - mExtents[i] > 0.0f || -e + mExtents[i] > 0.0f )
                return false;
            continue;
        }

        float s = (e - mExtents[i])/f;
        float t = (e + mExtents[i])/f;

        // fix order
        if ( s > t )
        {
            float temp = s;
            s = t;
            t = temp;
        }

        // adjust min and max values
        if ( s > maxS )
            maxS = s;
        if ( t < minT )
            minT = t;

        // check for intersection failure
        if ( maxS > minT )
            return false;
    }

    // done, have intersection
    return true;
}


//----------------------------------------------------------------------------
// @ OBB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between OBB and ray
//-----------------------------------------------------------------------------
Bool
OBB::Intersect( const Ray3& ray ) const
{
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // compute difference vector
    Vector3 diff = mCenter - ray.GetOrigin();

    // for each axis do
    for (int i = 0; i < 3; ++i)
    {
        // get axis i
        Vector3 axis = mRotation.GetColumn( i );

        // project relative vector onto axis
        float e = axis.Dot( diff );
        float f = ray.GetDirection().Dot( axis );

        // ray is parallel to plane
        if ( gfx::IsZero( f ) )
        {
            // ray passes by box
            if ( -e - mExtents[i] > 0.0f || -e + mExtents[i] > 0.0f )
                return false;
            continue;
        }

        float s = (e - mExtents[i])/f;
        float t = (e + mExtents[i])/f;

        // fix order
        if ( s > t )
        {
            float temp = s;
            s = t;
            t = temp;
        }

        // adjust min and max values
        if ( s > maxS )
            maxS = s;
        if ( t < minT )
            minT = t;

        // check for intersection failure
        if ( minT < 0.0f || maxS > minT )
            return false;
    }

    // done, have intersection
    return true;
}


//----------------------------------------------------------------------------
// @ OBB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between OBB and line segment
//-----------------------------------------------------------------------------
Bool
OBB::Intersect( const LineSegment3& segment ) const
{
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // compute difference vector
    Vector3 diff = mCenter - segment.GetOrigin();

    // for each axis do
    for (int i = 0; i < 3; ++i)
    {
        // get axis i
        Vector3 axis = mRotation.GetColumn( i );

        // project relative vector onto axis
        float e = axis.Dot( diff );
        float f = segment.GetDirection().Dot( axis );

        // ray is parallel to plane
        if ( IsZero( f ) )
        {
            // ray passes by box
            if ( -e - mExtents[i] > 0.0f || -e + mExtents[i] > 0.0f )
                return false;
            continue;
        }

        float s = (e - mExtents[i])/f;
        float t = (e + mExtents[i])/f;

        // fix order
        if ( s > t )
        {
            float temp = s;
            s = t;
            t = temp;
        }

        // adjust min and max values
        if ( s > maxS )
            maxS = s;
        if ( t < minT )
            minT = t;

        // check for intersection failure
        if ( minT < 0.0f || maxS > 1.0f || maxS > minT )
            return false;
    }

    // done, have intersection
    return true;

}   // End of OBB::Intersect()


//----------------------------------------------------------------------------
// @ OBB::Classify()
// ---------------------------------------------------------------------------
// Return signed distance between OBB and plane
//-----------------------------------------------------------------------------
float OBB::Classify( const Plane& plane ) const
{
    Vector3 xNormal = plane.GetNormal()*mRotation;
    // maximum extent in direction of plane normal 
    float r = Abs(mExtents.x*xNormal.x) 
            + Abs(mExtents.y*xNormal.y) 
            + Abs(mExtents.z*xNormal.z);
    // signed distance between box center and plane
    float d = plane.Test(mCenter);

    // return signed distance
    if (Abs(d) < r)
        return 0.0f;
    else if (d < 0.0f)
        return d + r;
    else
        return d - r;

}   // End of OBB::Classify()


//----------------------------------------------------------------------------
// @ ::Merge()
// ---------------------------------------------------------------------------
// Merge two OBBs together to create a new one
//-----------------------------------------------------------------------------
void
Merge( OBB& result, const OBB& b0, const OBB& b1 )
{
    // new center is average of original centers
    Vector3 newCenter = (b0.mCenter + b1.mCenter)*0.5f;

    // new axes are sum of rotations (as quaternions)
    Quat q0( b0.mRotation );
    Quat q1( b1.mRotation );
    Quat q;
    // this shouldn't happen, but just in case
    if ( q0.Dot(q1) < 0.0f )
        q = q0-q1;
    else
        q = q0+q1;
    q.Normalize();
    Matrix33 newRotation( q );

    // new extents are projections of old extents on new axes
    // for each axis do
    Vector3 newExtents;
    for (int i = 0; i < 3; ++i)
    {
        // get axis i
        Vector3 axis = newRotation.GetColumn( i );

        // get difference between this box center and other box center
        Vector3 centerDiff = b0.mCenter - newCenter;

        // rotate into box 0's space
        Vector3 xAxis = axis*b0.mRotation;
        // maximum extent in direction of axis
        newExtents[i] = Abs(centerDiff.x*axis.x) 
                      + Abs(centerDiff.y*axis.y)  
                      + Abs(centerDiff.z*axis.z) 
                      + Abs(xAxis.x*b0.mExtents.x) 
                      + Abs(xAxis.y*b0.mExtents.y) 
                      + Abs(xAxis.z*b0.mExtents.z);

        // get difference between this box center and other box center
        centerDiff = b1.mCenter - newCenter;

        // rotate into box 1's space
        xAxis = axis*b1.mRotation;
        // maximum extent in direction of axis
        float maxExtent = Abs(centerDiff.x*axis.x) 
                      + Abs(centerDiff.y*axis.y)  
                      + Abs(centerDiff.z*axis.z) 
                      + Abs(xAxis.x*b1.mExtents.x) 
                      + Abs(xAxis.y*b1.mExtents.y) 
                      + Abs(xAxis.z*b1.mExtents.z);
        // if greater than box0's result, use it
        if (maxExtent > result.mExtents[i])
            newExtents[i] = maxExtent;
    }

    // copy at end, in case user is passing in b0 or b1 as result
    result.mCenter = newCenter;
    result.mRotation = newRotation;
    result.mExtents = newExtents;

}   // End of ::Merge()

} // namespace gfx
