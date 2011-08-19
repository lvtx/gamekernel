#pragma once 

#include <kgraphics/math/Writer.h>

namespace gfx {

class Vector4
{
public:
    // constructor/destructor
    inline Vector4() {}
    inline Vector4( float _x, float _y, float _z, float _w ) :
        x(_x), y(_y), z(_z), w(_w)
    {
    }
    inline ~Vector4() {}

    // copy operations
    Vector4(const Vector4& other);
    Vector4& operator=(const Vector4& other);

    // text output (for debugging)
    friend Writer& operator<<(Writer& out, const Vector4& source);

    // accessors
    inline float& operator[]( unsigned int i )         { return (&x)[i]; }
    inline float operator[]( unsigned int i ) const    { return (&x)[i]; }

    float Length() const;
    float LengthSquared() const;

    // comparison
    bool operator==( const Vector4& other ) const;
    bool operator!=( const Vector4& other ) const;
    bool IsZero() const;
    bool IsUnit() const;

    // manipulators
    inline void Set( float _x, float _y, float _z, float _w );
    void Clean();       // sets near-zero elements to 0
    inline void Zero(); // sets all elements to 0
    void Normalize();   // sets to unit vector

    // operators

    // addition/subtraction
    Vector4 operator+( const Vector4& other ) const;
    Vector4& operator+=( const Vector4& other );
    Vector4 operator-( const Vector4& other ) const;
    Vector4& operator-=( const Vector4& other );

    // scalar multiplication
    Vector4    operator*( float scalar );
    friend Vector4    operator*( float scalar, const Vector4& vector );
    Vector4&          operator*=( float scalar );
    Vector4    operator/( float scalar );
    Vector4&          operator/=( float scalar );

    // dot product
    float              Dot( const Vector4& vector ) const;
    friend float       Dot( const Vector4& vector1, const Vector4& vector2 );

    // useful defaults
    static Vector4    xAxis;
    static Vector4    yAxis;
    static Vector4    zAxis;
    static Vector4    wAxis;
    static Vector4    origin;
        
    // member variables
    float x, y, z, w;

protected:

private:
};

//-------------------------------------------------------------------------------
// @ Vector4::Set()
//-------------------------------------------------------------------------------
// Set vector elements
//-------------------------------------------------------------------------------
inline void 
Vector4::Set( float _x, float _y, float _z, float _w )
{
    x = _x; y = _y; z = _z; w = _w;
}   // End of Vector4::Set()

//-------------------------------------------------------------------------------
// @ Vector4::Zero()
//-------------------------------------------------------------------------------
// Zero all elements
//-------------------------------------------------------------------------------
inline void 
Vector4::Zero()
{
    x = y = z = w = 0.0f;
}   // End of Vector4::Zero()

} // namespace gfx
