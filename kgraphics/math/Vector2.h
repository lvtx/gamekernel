#pragma once 

#include <kgraphics/math/Writer.h>

namespace gfx {

class Vector2
{
public:
    // constructor/destructor
    inline Vector2() {}
    inline Vector2( float _x, float _y ) :
        x(_x), y(_y)
    {
    }
    inline ~Vector2() {}
    
    // text output (for debugging)
    friend Writer& operator<<(Writer& out, const Vector2& source);
    
    // accessors
    inline float& operator[]( unsigned int i )          { return (&x)[i]; }
    inline float operator[]( unsigned int i ) const { return (&x)[i]; }

    float Length() const;
    float LengthSquared() const;

    // comparison
    bool operator==( const Vector2& other ) const;
    bool operator!=( const Vector2& other ) const;
    bool IsZero() const;

    // manipulators
    inline void Set( float _x, float _y );
    void Clean();       // sets near-zero elements to 0
    inline void Zero(); // sets all elements to 0
    void Normalize();   // sets to unit vector

    // operators
    Vector2 operator-() const;

    // addition/subtraction
    Vector2 operator+( const Vector2& other ) const;
    Vector2& operator+=( const Vector2& other );
    Vector2 operator-( const Vector2& other ) const;
    Vector2& operator-=( const Vector2& other );

    // scalar multiplication
    Vector2   operator*( float scalar );
    friend Vector2    operator*( float scalar, const Vector2& vector );
    Vector2&          operator*=( float scalar );
    Vector2   operator/( float scalar );
    Vector2&          operator/=( float scalar );

    // dot product
    float               Dot( const Vector2& vector ) const;
    friend float        Dot( const Vector2& vector1, const Vector2& vector2 );

    // perpendicular and cross product equivalent
    Vector2 Perpendicular() const { return Vector2(-y, x); } 
    float               PerpDot( const Vector2& vector ) const; 
    friend float        PerpDot( const Vector2& vector1, const Vector2& vector2 );


	Vector2&            RotationPos(float dir, float rad);


    // useful defaults
    static Vector2    xAxis;
    static Vector2    yAxis;
    static Vector2    origin;
    static Vector2    xy;
        
    // member variables
    float x, y;

protected:

private:
};

//-------------------------------------------------------------------------------
// @ Vector2::Set()
//-------------------------------------------------------------------------------
// Set vector elements
//-------------------------------------------------------------------------------
inline void 
Vector2::Set( float _x, float _y )
{
    x = _x; y = _y;
}   // End of Vector2::Set()

//-------------------------------------------------------------------------------
// @ Vector2::Zero()
//-------------------------------------------------------------------------------
// Zero all elements
//-------------------------------------------------------------------------------
inline void 
Vector2::Zero()
{
    x = y = 0.0f;
}   // End of Vector2::Zero()

} // namespace gfx
