#include <stdafx.h>

#include <kgraphics/math/Covariance.h>
#include <kgraphics/math/Matrix33.h>
#include <kgraphics/math/Vector3.h>
#include <kgraphics/math/math.h>
#include <string.h>

namespace gfx {

//-------------------------------------------------------------------------------
// @ ComputeCovarianceMatrix()
//-------------------------------------------------------------------------------
// Computes and returns the real, symmetric covariance matrix, along with the
// mean vector.
//-------------------------------------------------------------------------------
void ComputeCovarianceMatrix( Matrix33& C, Vector3& mean,
                              const Vector3* points, unsigned int numPoints )
{
    //ASSERT(numPoints > 1);

    mean = Vector3::origin;

    // compute the mean (the centroid of the points)
    unsigned int i;
    for (i = 0; i < numPoints; i++)
        mean += points[i];

    mean /= (float)numPoints;

    // compute the (co)variances
    float varX = 0.0f;
    float varY = 0.0f;
    float varZ = 0.0f;
    float covXY = 0.0f;
    float covXZ = 0.0f;
    float covYZ = 0.0f;

    for (i = 0; i < numPoints; i++)
    {
        Vector3 diff = points[i] - mean;
        
        varX += diff.x * diff.x;
        varY += diff.y * diff.y;
        varZ += diff.z * diff.z;
        covXY += diff.x * diff.y;
        covXZ += diff.x * diff.z;
        covYZ += diff.y * diff.z;
    }

    // divide all of the (co)variances by n - 1 
    // (skipping the division if n = 2, since that would be division by 1.0
    if (numPoints > 2)
    {
        const float normalize = (float)(numPoints - 1);
        varX /= normalize;
        varY /= normalize;
        varZ /= normalize;
        covXY /= normalize;
        covXZ /= normalize;
        covYZ /= normalize;
    }

    // pack values into the covariance matrix, which is symmetric
    C(0,0) = varX;
    C(1,1) = varY;
    C(2,2) = varZ;
    C(1,0) = C(0,1) = covXY;
    C(2,0) = C(0,2) = covXZ;
    C(1,2) = C(2,1) = covYZ;

}  // End of ComputeCovarianceMatrix()


//-------------------------------------------------------------------------------
// @ SymmetricHouseholder3x3()
//-------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------
static void SymmetricHouseholder3x3 (const Matrix33& M, Matrix33& Q, 
                                    Vector3& diag, Vector3& subd)
{
    // Computes the Householder reduction of M, computing:
    //
    // T = Q^t M Q
    //
    //   Input:   
    //     symmetric 3x3 matrix M
    //   Output:  
    //     orthogonal matrix Q
    //     diag, diagonal entries of T
    //     subd, lower-triangle entries of T (T is symmetric)

    // T will be stored as follows (because it is symmetric and tridiagonal):
    //
    // | diag[0]  subd[0]  0       |
    // | subd[0]  diag[1]  subd[1] |
    // | 0        subd[1]  diag[2] |

    diag[0] = M(0,0); // in both cases below, T(0,0) = M(0,0) 
    subd[2] = 0; // T is to be a tri-diagonal matrix - the (2,0) and (0,2) 
                 // entries must be zero, so we don't need subd[2] for this step

    // so we know that T will actually be:
    //
    // | M(0,0)   subd[0]  0       |
    // | subd[0]  diag[1]  subd[1] |
    // | 0        subd[1]  diag[2] |

    // so the only question remaining is the lower-right block and subd[0]

    if ( Abs(M(2,0)) < kEpsilon )
    {
        // if M(2,0) (and thus M(0,2)) is zero, then the matrix is already in
        // tridiagonal form.  As such, the Q matix is the identity, and we
        // just extract the diagonal and subdiagonal components of T as-is
        Q.Identity();

        // so we see that T will actually be:
        //
        // | M(0,0)  M(1,0)  0      |
        // | M(1,0)  M(1,1)  M(2,1) |
        // | 0       M(2,1)  M(2,2) |
        diag[1] = M(1,1);
        diag[2] = M(2,2);

        subd[0] = M(1,0);
        subd[1] = M(2,1);
    }
    else
    {
        // grab the lower triangle of the matrix, minus a, which we don't need
        // (see above)
        // |       |
        // | b d   |
        // | c e f |
        const float b = M(1,0);
        const float c = M(2,0);
        const float d = M(1,1);
        const float e = M(2,1);
        const float f = M(2,2);

        // normalize b and c to unit length and store in u and v
        // we want the lower-right block we create to be orthonormal
        const float L = Sqrt(b * b + c * c);
        const float u = b / L;
        const float v = c / L;
        Q.SetRows(Vector3(1.0f, 0.0f, 0.0f),
                  Vector3(0.0f, u,    v),
                  Vector3(0.0f, v,    -u));

        float q = 2 * u * e + v * (f - d);
        diag[1] = d + v * q;
        diag[2] = f - v * q;

        subd[0] = L;
        subd[1] = e - u * q;

        // so we see that T will actually be:
        //
        // | M(0,0)  L       0     |
        // | L       d+c*q   e-b*q |
        // | 0       e-b*q   f-c*q |
    }
}  // End of SymmetricHouseholder3x3

//-------------------------------------------------------------------------------
// @ QLAlgorithm()
//-------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------
static int QLAlgorithm (Matrix33& M, Vector3& diag, Vector3& subd)
{
    // QL iteration with implicit shifting to reduce matrix from tridiagonal
    // to diagonal

    int L;
    for (L = 0; L < 3; L++)
    {
        // As this is an iterative process, we need to keep a maximum number of
        // iterations, just in case something is wrong - we cannot afford to
        // loop forever
        const int maxIterations = 32;

        int iter;
        for (iter = 0; iter < maxIterations; iter++)
        {
            int m;
            for (m = L; m <= 1; m++)
            {
                float dd = Abs(diag[m]) + Abs(diag[m+1]);
                if ( Abs(subd[m]) + dd == dd )
                    break;
            }
            if ( m == L )
                break;

            float g = (diag[L+1]-diag[L])/(2*subd[L]);
            float r = Sqrt(g*g+1);
            if ( g < 0 )
                g = diag[m]-diag[L]+subd[L]/(g-r);
            else
                g = diag[m]-diag[L]+subd[L]/(g+r);
            float s = 1, c = 1, p = 0;
            for (int i = m-1; i >= L; i--)
            {
                float f = s*subd[i], b = c*subd[i];
                if ( Abs(f) >= Abs(g) )
                {
                    c = g/f;
                    r = Sqrt(c*c+1);
                    subd[i+1] = f*r;
                    c *= (s = 1/r);
                }
                else
                {
                    s = f/g;
                    r = Sqrt(s*s+1);
                    subd[i+1] = g*r;
                    s *= (c = 1/r);
                }
                g = diag[i+1]-p;
                r = (diag[i]-g)*s+2*b*c;
                p = s*r;
                diag[i+1] = g+p;
                g = c*r-b;

                for (int k = 0; k < 3; k++)
                {
                    f = M(k,i+1);
                    M(k,i+1) = s*M(k,i)+c*f;
                    M(k,i) = c*M(k,i)-s*f;
                }
            }
            diag[L] -= p;
            subd[L] = g;
            subd[m] = 0;
        }

        // exceptional case - matrix took more iterations than should be 
        // possible to move to diagonal form
        if ( iter == maxIterations )
            return 0;
    }

    return 1;
}  // End of QLAlgorithm

//-------------------------------------------------------------------------------
// @ GetRealSymmetricEigenvectors()
//-------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------
void GetRealSymmetricEigenvectors( Vector3& v1, Vector3& v2, Vector3& v3, 
                                   const Matrix33& A )
{    
    Vector3 eigenvalues;
    Vector3 lowerTriangle;
    Matrix33 Q;

    SymmetricHouseholder3x3 (A, Q, eigenvalues, lowerTriangle);
    QLAlgorithm(Q, eigenvalues, lowerTriangle);

    // Sort the eigenvalues from greatest to smallest, and use these indices
    // to sort the eigenvectors
    int v1Index, v2Index, v3Index;
    if (eigenvalues[0] > eigenvalues[1])
    {
        if (eigenvalues[1] > eigenvalues[2])
        {
            v1Index = 0;
            v2Index = 1;
            v3Index = 2;
        }
        else if (eigenvalues[2] > eigenvalues[0])
        {
            v1Index = 2;
            v2Index = 0;
            v3Index = 1;
        }
        else
        {
            v1Index = 0;
            v2Index = 2;
            v3Index = 1;
        }
    }
    else // eigenvalues[1] >= eigenvalues[0]
    {
        if (eigenvalues[0] > eigenvalues[2])
        {
            v1Index = 1;
            v2Index = 0;
            v3Index = 2;
        }
        else if (eigenvalues[2] > eigenvalues[1])
        {
            v1Index = 2;
            v2Index = 1;
            v3Index = 0;
        }
        else
        {
            v1Index = 1;
            v2Index = 2;
            v3Index = 0;
        }
    }

    // Sort the eigenvectors into the output vectors
    v1 = Q.GetColumn(v1Index);
    v2 = Q.GetColumn(v2Index);
    v3 = Q.GetColumn(v3Index);

    // If the resulting eigenvectors are left-handed, negate the 
    // min-eigenvalue eigenvector to make it right-handed
    if ( v1.Dot(v2.Cross(v3)) < 0.0f )
        v3 = -v3;
}  // End of GetRealSymmetricEigenvectors

} // namespace gfx
