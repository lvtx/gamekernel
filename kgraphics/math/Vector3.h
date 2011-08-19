#pragma once 

#include <kgraphics/math/Writer.h>
#include <kgraphics/math/math.h>

namespace gfx {

class Vector3
{
public:
    // constructor/destructor
	inline Vector3() : x(0.f), y(0.f), z(0.f) {}

    inline Vector3( float _x, float _y, float _z ) :
        x(_x), y(_y), z(_z)
    {
    }
    inline ~Vector3() {}

    // copy operations
    Vector3(const Vector3& other);
    Vector3& operator=(const Vector3& other);

    // text output (for debugging)
    friend Writer& operator<<(Writer& out, const Vector3& source);

    // accessors
    inline float& operator[]( unsigned int i )          { return (&x)[i]; }
    inline float operator[]( unsigned int i ) const { return (&x)[i]; }

    float Length() const;
    float LengthSquared() const;

    friend float Distance( const Vector3& p0, const Vector3& p1 );
	friend float Distance2D( const Vector3& p0, const Vector3& p1 );
    friend float DistanceSquared( const Vector3& p0, const Vector3& p1 );

    // comparison
    bool operator==( const Vector3& other ) const;
    bool operator!=( const Vector3& other ) const;
    bool IsZero() const;
    bool IsUnit() const;

    // manipulators
    inline void Set( float _x, float _y, float _z );
    void Clean();       // sets near-zero elements to 0
    inline void Zero(); // sets all elements to 0
    void Normalize();   // sets to unit vector

    // operators

    // addition/subtraction
    Vector3 operator+( const Vector3& other ) const;
    friend Vector3& operator+=( Vector3& vector, const Vector3& other );
    Vector3 operator-( const Vector3& other ) const;
    friend Vector3& operator-=( Vector3& vector, const Vector3& other );

    Vector3 operator-() const;

    // scalar multiplication
    Vector3   operator*( float scalar );
    friend Vector3    operator*( float scalar, const Vector3& vector );
    Vector3&          operator*=( float scalar );
    Vector3   operator/( float scalar );
    Vector3&          operator/=( float scalar );

    // dot product/cross product
    float               Dot( const Vector3& vector ) const;
    friend float        Dot( const Vector3& vector1, const Vector3& vector2 );
    Vector3           Cross( const Vector3& vector ) const;
    friend Vector3    Cross( const Vector3& vector1, const Vector3& vector2 );


	inline void rotateXYBy(Float Rad, const Vector3& center)
	{
		float cs = cos(Rad);
		float sn = sin(Rad);
		x -= center.x;
		y -= center.y;
		Set((x*cs - y*sn), (x*sn + y*cs), z);
		x += center.x;
		y += center.y;
	}


    // useful defaults
    static Vector3    xAxis;
    static Vector3    yAxis;
    static Vector3    zAxis;
    static Vector3    origin;

    // member variables
    float x, y, z;

protected:

private:
};

//-------------------------------------------------------------------------------
// @ Vector3::Set()
//-------------------------------------------------------------------------------
// Set vector elements
//-------------------------------------------------------------------------------
inline void 
Vector3::Set( float _x, float _y, float _z )
{
    x = _x; y = _y; z = _z;
}   // End of Vector3::Set()

//-------------------------------------------------------------------------------
// @ Vector3::Zero()
//-------------------------------------------------------------------------------
// Zero all elements
//-------------------------------------------------------------------------------
inline void 
Vector3::Zero()
{
    x = y = z = 0.0f;
}   // End of Vector3::Zero()

} // namespace gfx
