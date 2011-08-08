#pragma once

namespace gfx {

class Vector3;
class Matrix33;

void ComputeCovarianceMatrix( Matrix33& C, Vector3& mean, 
							  const Vector3* points, unsigned int numPoints );

void GetRealSymmetricEigenvectors( Vector3& v1, Vector3& v2, Vector3& v3, const Matrix33& A );

} // namespace gfx
