//===============================================================================
// @ AABB.cpp
// 
// Axis-aligned bounding box class
// ------------------------------------------------------------------------------
// Copyright (C) 2004 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <stdafx.h>
#include <kgraphics/math/AABB.h>
#include <kgraphics/math/Vector3.h>
#include <kgraphics/math/Line3.h>
#include <kgraphics/math/Ray3.h>
#include <kgraphics/math/LineSegment3.h>
#include <kgraphics/math/Plane.h>
#include <kgraphics/math/math.h>


namespace gfx { 

//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
AABB::AABB(const AABB& other) : 
    mMinima( other.mMinima ),
    mMaxima( other.mMaxima )
{

}   // End of AABB::AABB()


//-------------------------------------------------------------------------------
// @ AABB::operator=()
//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
AABB&
AABB::operator=(const AABB& other)
{
    // if same object
    if ( this == &other )
        return *this;
        
    mMinima = other.mMinima;
    mMaxima = other.mMaxima;
    
    return *this;

}   // End of AABB::operator=()


//-------------------------------------------------------------------------------
// @ operator<<()
//-------------------------------------------------------------------------------
// Text output for debugging
//-------------------------------------------------------------------------------
Writer& 
operator<<(Writer& out, const AABB& source)
{
    out << source.mMinima;
    out << ' ' << source.mMaxima;

    return out;
    
}   // End of operator<<()
    

//-------------------------------------------------------------------------------
// @ AABB::operator==()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
Bool 
AABB::operator==( const AABB& other ) const
{
    if ( other.mMinima == mMinima && other.mMaxima == mMaxima )
        return true;

    return false;   
}   // End of AABB::operator==()


//-------------------------------------------------------------------------------
// @ AABB::operator!=()
//-------------------------------------------------------------------------------
// Comparison operator
//-------------------------------------------------------------------------------
Bool 
AABB::operator!=( const AABB& other ) const
{
    if ( other.mMinima != mMinima || other.mMaxima != mMaxima )
        return true;

    return false;

}   // End of AABB::operator!=()


//-------------------------------------------------------------------------------
// @ AABB::Set()
//-------------------------------------------------------------------------------
// Set AABB based on set of points
//-------------------------------------------------------------------------------
void
AABB::Set( const Vector3* points, unsigned int numPoints )
{
    // compute minimal and maximal bounds
    mMinima = points[0];
    mMaxima = points[0];
    for ( unsigned int i = 1; i < numPoints; ++i )
    {
        if (points[i].x < mMinima.x)
            mMinima.x = points[i].x;
        else if (points[i].x > mMaxima.x )
            mMaxima.x = points[i].x;
        if (points[i].y < mMinima.y)
            mMinima.y = points[i].y;
        else if (points[i].y > mMaxima.y )
            mMaxima.y = points[i].y;
        if (points[i].z < mMinima.z)
            mMinima.z = points[i].z;
        else if (points[i].z > mMaxima.z )
            mMaxima.z = points[i].z;
    }
}   // End of AABB::Set()


//-------------------------------------------------------------------------------
// @ AABB::AddPoint()
//-------------------------------------------------------------------------------
// Add a point to the AABB
//-------------------------------------------------------------------------------
void
AABB::AddPoint( const Vector3& point )
{
    if (point.x < mMinima.x)
        mMinima.x = point.x;
    else if (point.x > mMaxima.x )
        mMaxima.x = point.x;
    if (point.y < mMinima.y)
        mMinima.y = point.y;
    else if (point.y > mMaxima.y )
        mMaxima.y = point.y;
    if (point.z < mMinima.z)
        mMinima.z = point.z;
    else if (point.z > mMaxima.z )
        mMaxima.z = point.z;

}   // End of AABB::AddPoint()



//----------------------------------------------------------------------------
// @ AABB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between AABB and AABB
//-----------------------------------------------------------------------------
Bool 
AABB::Intersect( const AABB& other ) const
{
    // if separated in x direction
    if (mMinima.x > other.mMaxima.x || other.mMinima.x > mMaxima.x )
        return false;

    // if separated in y direction
    if (mMinima.y > other.mMaxima.y || other.mMinima.y > mMaxima.y )
        return false;

    // if separated in z direction
    if (mMinima.z > other.mMaxima.z || other.mMinima.z > mMaxima.z )
        return false;

    // no separation, must be intersecting
    return true;
}


//----------------------------------------------------------------------------
// @ AABB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between AABB and line
//-----------------------------------------------------------------------------
Bool
AABB::Intersect( const Line3& line ) const
{
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // do tests against three sets of planes
    for ( int i = 0; i < 3; ++i )
    {
        // line is parallel to plane
        if ( IsZero( line.GetDirection()[i] ) )
        {
            // line passes by box
            if ( (line.GetOrigin())[i] < mMinima[i] || (line.GetOrigin())[i] > mMaxima[i] )
                return false;
        }
        else
        {
            // compute intersection parameters and sort
            float s = (mMinima[i] - line.GetOrigin()[i])/line.GetDirection()[i];
            float t = (mMaxima[i] - line.GetOrigin()[i])/line.GetDirection()[i];
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
    }

    // done, have intersection
    return true;
}


//----------------------------------------------------------------------------
// @ AABB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between AABB and ray
//-----------------------------------------------------------------------------
Bool
AABB::Intersect( const Ray3& ray ) const
{
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // do tests against three sets of planes
    for ( int i = 0; i < 3; ++i )
    {
        // ray is parallel to plane
        if ( IsZero( ray.GetDirection()[i] ) )
        {
            // ray passes by box
            if ( (ray.GetOrigin())[i] < mMinima[i] || (ray.GetOrigin())[i] > mMaxima[i] )
                return false;
        }
        else
        {
            // compute intersection parameters and sort
            float s = (mMinima[i] - ray.GetOrigin()[i])/ray.GetDirection()[i];
            float t = (mMaxima[i] - ray.GetOrigin()[i])/ray.GetDirection()[i];
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
    }

    // done, have intersection
    return true;
}


//----------------------------------------------------------------------------
// @ AABB::Intersect()
// ---------------------------------------------------------------------------
// Determine intersection between AABB and line segment
//-----------------------------------------------------------------------------
Bool
AABB::Intersect( const LineSegment3& segment ) const
{
    float maxS = -FLT_MAX;
    float minT = FLT_MAX;

    // do tests against three sets of planes
    for ( int i = 0; i < 3; ++i )
    {
        // segment is parallel to plane
        if ( IsZero( segment.GetDirection()[i] ) )
        {
            // segment passes by box
            if ( (segment.GetOrigin())[i] < mMinima[i] || (segment.GetOrigin())[i] > mMaxima[i] )
                return false;
        }
        else
        {
            // compute intersection parameters and sort
            float s = (mMinima[i] - segment.GetOrigin()[i])/segment.GetDirection()[i];
            float t = (mMaxima[i] - segment.GetOrigin()[i])/segment.GetDirection()[i];
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
    }

    // done, have intersection
    return true;
}   


//----------------------------------------------------------------------------
// @ AABB:Classify()
// ---------------------------------------------------------------------------
// Compute signed distance between AABB and plane
//-----------------------------------------------------------------------------
float
AABB::Classify( const Plane& plane ) const
{
    Vector3 diagMin, diagMax;
    // set min/max values for x direction
    if ( plane.GetNormal().x >= 0.0f )
    {
        diagMin.x = mMinima.x;
        diagMax.x = mMaxima.x;
    }
    else
    {
        diagMin.x = mMinima.x;
        diagMax.x = mMaxima.x;
    }

    // set min/max values for y direction
    if ( plane.GetNormal().x >= 0.0f )
    {
        diagMin.y = mMinima.y;
        diagMax.y = mMaxima.y;
    }
    else
    {
        diagMin.y = mMinima.y;
        diagMax.y = mMaxima.y;
    }

    // set min/max values for z direction
    if ( plane.GetNormal().z >= 0.0f )
    {
        diagMin.z = mMinima.z;
        diagMax.z = mMaxima.z;
    }
    else
    {
        diagMin.z = mMinima.z;
        diagMax.z = mMaxima.z;
    }

    // minimum on positive side of plane, box on positive side
    float test = plane.Test( diagMin );
    if ( test > 0.0f )
        return test;

    test = plane.Test( diagMax );
    // min on non-positive side, max on non-negative side, intersection
    if ( test >= 0.0f )
        return 0.0f;
    // max on negative side, box on negative side
    else
        return test;

}   // End of AABB::Classify()


//----------------------------------------------------------------------------
// @ ::Merge()
// ---------------------------------------------------------------------------
// Merge two AABBs together to create a new one
//-----------------------------------------------------------------------------
void
Merge( AABB& result, const AABB& b0, const AABB& b1 )
{
    Vector3 newMinima( b0.mMinima );
    Vector3 newMaxima( b0.mMaxima );

    if (b1.mMinima.x < newMinima.x)
        newMinima.x = b1.mMinima.x;
    if (b1.mMinima.y < newMinima.y)
        newMinima.y = b1.mMinima.y;
    if (b1.mMinima.z < newMinima.z)
        newMinima.z = b1.mMinima.z;

    if (b1.mMaxima.x > newMaxima.x )
        newMaxima.x = b1.mMaxima.x;
    if (b1.mMaxima.y > newMaxima.y )
        newMaxima.y = b1.mMaxima.y;
    if (b1.mMaxima.z > newMaxima.z )
        newMaxima.z = b1.mMaxima.z;

    // set new box
    result.mMinima = newMinima;
    result.mMaxima = newMaxima;

}   // End of ::Merge()

} // namespace gfx
