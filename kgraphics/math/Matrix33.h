#pragma once 

#include <kgraphics/math/Writer.h>

namespace gfx {

class Quat;
class Vector3;

/**
 * @class Matrix33
 *
 */
class Matrix33
{
public:
    // constructor/destructor
    inline Matrix33() {}
    inline ~Matrix33() {}
    explicit Matrix33( const Quat& quat );
    
    // copy operations
    Matrix33(const Matrix33& other);
    Matrix33& operator=(const Matrix33& other);

    // text output (for debugging)
    friend Writer& operator<<(Writer& out, const Matrix33& source);

    // accessors
    inline float& operator()(unsigned int i, unsigned int j);
    inline float operator()(unsigned int i, unsigned int j) const;

    // comparison
    Bool operator==( const Matrix33& other ) const;
    Bool operator!=( const Matrix33& other ) const;
    Bool IsZero() const;
    Bool IsIdentity() const;

    // manipulators
    void SetRows( const Vector3& row1, const Vector3& row2, const Vector3& row3 ); 
    void GetRows( Vector3& row1, Vector3& row2, Vector3& row3 ) const; 
    Vector3 GetRow( unsigned int i ) const; 

    void SetColumns( const Vector3& col1, const Vector3& col2, const Vector3& col3 ); 
    void GetColumns( Vector3& col1, Vector3& col2, Vector3& col3 ) const; 
    Vector3 GetColumn( unsigned int i ) const; 

    void Clean();
    void Identity();

    Matrix33& Inverse();
    friend Matrix33 Inverse( const Matrix33& mat );

    Matrix33& Transpose();
    friend Matrix33 Transpose( const Matrix33& mat );

    // useful computations
    Matrix33 Adjoint() const;
    float Determinant() const;
    float Trace() const;
        
    // transformations
    Matrix33& Rotation( const Quat& rotate );
    Matrix33& Rotation( float zRotation, float yRotation, float xRotation );
    Matrix33& Rotation( const Vector3& axis, float angle );

    Matrix33& Scaling( const Vector3& scale );

    Matrix33& RotationX( float angle );
    Matrix33& RotationY( float angle );
    Matrix33& RotationZ( float angle );

    void GetFixedAngles( float& zRotation, float& yRotation, float& xRotation );
    void GetAxisAngle( Vector3& axis, float& angle );

    // operators

    // addition and subtraction
    Matrix33 operator+( const Matrix33& other ) const;
    Matrix33& operator+=( const Matrix33& other );
    Matrix33 operator-( const Matrix33& other ) const;
    Matrix33& operator-=( const Matrix33& other );

    Matrix33 operator-() const;

    // multiplication
    Matrix33& operator*=( const Matrix33& matrix );
    Matrix33 operator*( const Matrix33& matrix ) const;

    // column vector multiplier
    Vector3 operator*( const Vector3& vector ) const;
    // row vector multiplier
    friend Vector3 operator*( const Vector3& vector, const Matrix33& matrix );

    Matrix33& operator*=( float scalar );
    friend Matrix33 operator*( float scalar, const Matrix33& matrix );
    Matrix33 operator*( float scalar ) const;

    // low-level data accessors - implementation-dependent
    operator float*() { return mV; }
    operator const float*() const { return mV; }

    // member variables
    float mV[9];

protected:

private:
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ Matrix33::operator()()
//-------------------------------------------------------------------------------
// 2D array accessor
//-------------------------------------------------------------------------------
inline float&
Matrix33::operator()(unsigned int i, unsigned int j)
{
   return mV[i + 3*j];

}   // End of Matrix33::operator()()

//-------------------------------------------------------------------------------
// @ Matrix33::operator()()
//-------------------------------------------------------------------------------
// 2D array accessor
//-------------------------------------------------------------------------------
inline float
Matrix33::operator()(unsigned int i, unsigned int j) const
{
   return mV[i + 3*j];

}   // End of Matrix33::operator()()

} // namespace gfx
