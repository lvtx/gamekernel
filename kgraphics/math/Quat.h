#pragma once 

#include <kgraphics/math/Writer.h>

namespace gfx {

class Vector3;
class Matrix33;

class Quat
{
public:
    // constructor/destructor
    inline Quat() : w(1.0f), x(0.0f), y(0.0f), z(0.0f)
    {}
    inline Quat( float _w, float _x, float _y, float _z ) :
        w(_w), x(_x), y(_y), z(_z)
    {
    }
    Quat( const Vector3& axis, float angle );
    Quat( const Vector3& from, const Vector3& to );
    explicit Quat( const Vector3& vector );
    explicit Quat( const Matrix33& rotation );
    inline ~Quat() {}

    // copy operations
    Quat(const Quat& other);
    Quat& operator=(const Quat& other);

    // text output (for debugging)
    friend Writer& operator<<(Writer& out, const Quat& source);

    // accessors
    inline float& operator[]( unsigned int i )         { return (&x)[i]; }
    inline float operator[]( unsigned int i ) const    { return (&x)[i]; }

    float Magnitude() const;
    float Norm() const;

    // comparison
    bool operator==( const Quat& other ) const;
    bool operator!=( const Quat& other ) const;
    bool IsZero() const;
    bool IsUnit() const;
    bool IsIdentity() const;

    // manipulators
    inline void Set( float _w, float _x, float _y, float _z );
    void Set( const Vector3& axis, float angle );
    void Set( const Vector3& from, const Vector3& to );
    void Set( const Matrix33& rotation );
    void Set( float zRotation, float yRotation, float xRotation ); 

    void GetAxisAngle( Vector3& axis, float& angle );

    void Clean();       // sets near-zero elements to 0
    inline void Zero(); // sets all elements to 0
    void Normalize();   // sets to unit quaternion
    inline void Identity();    // sets to identity quaternion

    // complex conjugate
    friend Quat Conjugate( const Quat& quat );
    const Quat& Conjugate();

    // invert quaternion
    friend Quat Inverse( const Quat& quat );
    const Quat& Inverse();

    // operators

    // addition/subtraction
    Quat operator+( const Quat& other ) const;
    Quat& operator+=( const Quat& other );
    Quat operator-( const Quat& other ) const;
    Quat& operator-=( const Quat& other );

    Quat operator-() const;

    // scalar multiplication
    friend Quat    operator*( float scalar, const Quat& vector );
    Quat&          operator*=( float scalar );

    // quaternion multiplication
    Quat operator*( const Quat& other ) const;
    Quat& operator*=( const Quat& other );

    // dot product
    float              Dot( const Quat& vector ) const;
    friend float       Dot( const Quat& vector1, const Quat& vector2 );

    // vector rotation
    Vector3   Rotate( const Vector3& vector ) const;

    // interpolation
    friend void Lerp( Quat& result, const Quat& start, const Quat& end, float t );
    friend void Slerp( Quat& result, const Quat& start, const Quat& end, float t );
    friend void ApproxSlerp( Quat& result, const Quat& start, const Quat& end, float t );

    // useful defaults
    static Quat   zero;
    static Quat   identity;
        
    // member variables
    float w, x, y, z;      

protected:

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ Quat::Set()
//-------------------------------------------------------------------------------
// Set vector elements
//-------------------------------------------------------------------------------
inline void 
Quat::Set( float _w, float _x, float _y, float _z )
{
    w = _w; x = _x; y = _y; z = _z;
}   // End of Quat::Set()

//-------------------------------------------------------------------------------
// @ Quat::Zero()
//-------------------------------------------------------------------------------
// Zero all elements
//-------------------------------------------------------------------------------
inline void 
Quat::Zero()
{
    x = y = z = w = 0.0f;
}   // End of Quat::Zero()

//-------------------------------------------------------------------------------
// @ Quat::Identity()
//-------------------------------------------------------------------------------
// Set to identity quaternion
//-------------------------------------------------------------------------------
inline void 
Quat::Identity()
{
    x = y = z = 0.0f;
    w = 1.0f;
}   // End of Quat::Identity

} // namespace gfx
