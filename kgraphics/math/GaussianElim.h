#pragma once 

namespace gfx { 
// NOTE: matrix mustbe be stored in column major order

// solve for system Ax = b for n by n matrix and n-vector
// will destroy contents of A and return result in b
Bool Solve( float* b, float* A, unsigned int n );

// invert matrix n by n matrix A
// will store results in A
Bool InvertMatrix( float* A, unsigned int n );


// Get determinant of matrix using Gaussian elimination
// Will not destroy A
float Determinant( float* A, unsigned int n );


} // namespace gfx
