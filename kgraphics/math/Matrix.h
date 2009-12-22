#pragma once 

#include <kgraphics/math/Euler.h>
#include <kgraphics/math/Vector.h>

namespace gfx {
/** 
 * @class Matrix 
 *
 * @brief 행렬의 기본 기능을 갖는다.
 */
class Matrix 
{
public:
	/// 단위 행렬로 행렬 생성
	explicit Matrix();

	/// 회전 값들로 행렬 생성. 기본축
	explicit Matrix(const Euler& e);
	
	/// angle 값 만큼 dir을 축으로 회전한 행렬
	Matrix(const Vector3& dir, float angle);

	D3DXMATRIX* Get()
	{
		return &m_;
	}

	const D3DXMATRIX* GetConst() const
	{
		return &m_;
	}

	/// 단위행렬로 초기화
	void Identity();

	/// 행렬식 값
	float Determinent();

	/// 역행렬. 역행렬이 없으면 아무 동작이 없다.
	void Inverse();

	/// 전치 행렬
	void Transpose();

	/// 행렬 곱하기. 결과 값을 r에 저장		 
	void Multiply( const Matrix& m, Matrix& r );

	/// 이동. v 만큼 이동한다. 누적되지 않는다.
	void Translate(const Vector3& v);

	/// 확대. p의 값을 x, y, z 방향 확대로 사용
	void Scale(const Vector3& p);

	/// 같은 값의 확대
	void Scale(const float f);

	/// X 축 회전으로 초기화한다.
	void RotateX(float angle);

	/// Y 축 회전으로 초기화한다.
	void RotateY(float angle);

	/// Z 축 회전으로 초기화한다.
	void RotateZ(float angle);

	/// 카메라 지정
	void LookAtLH(const Vector3& eye, const Vector3& lookAt, const Vector3& up);

	/// 프로젝션을 지정
	void PerspectiveFovLH(float angle, float aspect, float nz, float fz);

	void OrthoLH( float width, float height, float nz, float fz );

public:
	D3DXMATRIX m_;
};

} // namespace gfx


