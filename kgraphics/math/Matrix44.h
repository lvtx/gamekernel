#pragma once

#include <kgraphics/math/Writer.h>
#include <kgraphics/math/Vector3.h>
#include <kgraphics/math/Vector4.h>

namespace gfx {

class Quat;
class Matrix33;
class Vector3;

class Matrix44
{
public:
    // constructor/destructor
    inline Matrix44() { Identity(); }
    inline ~Matrix44() {}
    explicit Matrix44( const Quat& quat );
    explicit Matrix44( const Matrix33& matrix );
    
    // copy operations
    Matrix44(const Matrix44& other);
    Matrix44& operator=(const Matrix44& other);

    // text output (for debugging)
    friend Writer& operator<<(Writer& out, const Matrix44& source);

    // accessors
    float &operator()(unsigned int i, unsigned int j);
    float operator()(unsigned int i, unsigned int j) const;

    // comparison
    bool operator==( const Matrix44& other ) const;
    bool operator!=( const Matrix44& other ) const;
    bool IsZero() const;
    bool IsIdentity() const;

    // manipulators
    void SetRows( const Vector4& row1, const Vector4& row2, 
                  const Vector4& row3, const Vector4& row4 ); 
    void GetRows( Vector4& row1, Vector4& row2, Vector4& row3, Vector4& row4 ); 

    void SetColumns( const Vector4& col1, const Vector4& col2, 
                     const Vector4& col3, const Vector4& col4 ); 
    void GetColumns( Vector4& col1, Vector4& col2, Vector4& col3, Vector4& col4 ); 

    void Clean();
    void Identity();

    Matrix44& AffineInverse();
    friend Matrix44 AffineInverse( const Matrix44& mat );

    Matrix44& Transpose();
    friend Matrix44 Transpose( const Matrix44& mat );
        
    // transformations
    Matrix44& Translation( const Vector3& xlate );
    Matrix44& Rotation( const Matrix33& matrix );
    Matrix44& Rotation( const Quat& rotate );
    Matrix44& Rotation( float zRotation, float yRotation, float xRotation );
    Matrix44& Rotation( const Vector3& axis, float angle );

    Matrix44& Scaling( const Vector3& scale );

    Matrix44& RotationX( float angle );
    Matrix44& RotationY( float angle );
    Matrix44& RotationZ( float angle );

    void GetFixedAngles( float& zRotation, float& yRotation, float& xRotation );
    void GetAxisAngle( Vector3& axis, float& angle );

    // operators

    // addition and subtraction
    Matrix44 operator+( const Matrix44& other ) const;
    Matrix44& operator+=( const Matrix44& other );
    Matrix44 operator-( const Matrix44& other ) const;
    Matrix44& operator-=( const Matrix44& other );

    Matrix44 operator-() const;

    // multiplication
    Matrix44& operator*=( const Matrix44& matrix );
    Matrix44 operator*( const Matrix44& matrix ) const;

    // column vector multiplier
    Vector4 operator*( const Vector4& vector ) const;
    // row vector multiplier
    friend Vector4 operator*( const Vector4& vector, const Matrix44& matrix );

    // scalar multiplication
    Matrix44& operator*=( float scalar );
    friend Matrix44 operator*( float scalar, const Matrix44& matrix );
    Matrix44 operator*( float scalar ) const;

    // vector3 ops
    Vector3 Transform( const Vector3& point ) const;

    // point ops
    Vector3 TransformPoint( const Vector3& point ) const;

    // low-level data accessors - implementation-dependent
    operator float*() { return mV; }
    operator const float*() const { return mV; }

    // member variables
    float mV[16];

protected:

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ Matrix44::operator()()
//-------------------------------------------------------------------------------
// 2D array accessor
//-------------------------------------------------------------------------------
inline float&
Matrix44::operator()(unsigned int i, unsigned int j)
{
   return mV[i + 4*j];

}   // End of Matrix44::operator()()

//-------------------------------------------------------------------------------
// @ Matrix44::operator()()
//-------------------------------------------------------------------------------
// 2D array accessor
//-------------------------------------------------------------------------------
inline float
Matrix44::operator()(unsigned int i, unsigned int j) const
{
   return mV[i + 4*j];

}   // End of Matrix44::operator()()

} // namespace gfx
