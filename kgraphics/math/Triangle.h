#pragma once 

namespace gfx {

class Vector2;
class Vector3;
class Ray3;
class Plane;


// 3D versions
bool IsPointInTriangle( const Vector3& point, const Vector3& P0,
                        const Vector3& P1, const Vector3& P2 );
void BarycentricCoordinates( float &r, float &s, float& t, 
                             const Vector3& point, const Vector3& P0,
                             const Vector3& P1, const Vector3& P2 );

// intersection
bool TriangleIntersect( const Vector3& P0, const Vector3& P1, 
                        const Vector3& P2, const Vector3& Q0, 
                        const Vector3& Q1, const Vector3& Q2 );
bool TriangleIntersect( float& t, const Vector3& P0, const Vector3& P1, 
                        const Vector3& P2, const Ray3& ray );

// plane classification
float TriangleClassify( const Vector3& P0, const Vector3& P1, 
                        const Vector3& P2, const Plane& plane );

// 2D versions
bool IsPointInTriangle( const Vector2& point, const Vector2& P0,
                        const Vector2& P1, const Vector2& P2 );
void BarycentricCoordinates( float &r, float &s, float& t, 
                             const Vector2& point, const Vector2& P0,
                             const Vector2& P1, const Vector2& P2 );
bool TriangleIntersect( const Vector2& P0, const Vector2& P1, 
                        const Vector2& P2, const Vector2& Q0, 
                        const Vector2& Q1, const Vector2& Q2 );


} // namespace gfx
