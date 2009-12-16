#include "stdafx.h"

#include "Matrix.h"

using namespace gfx;

Matrix::Matrix()
{
	Identity();
}

Matrix::Matrix(const Euler& e)
{
	Identity();
	// 비행기가 X 축 방향으로 놓여있다.
	D3DXMatrixRotationX(&m_, e.GetRoll());
	D3DXMatrixRotationY(&m_, e.GetYaw());
	D3DXMatrixRotationZ(&m_, e.GetPitch());
}

Matrix::Matrix(const Vector3& dir, float angle)
{
	Identity();
	D3DXMatrixRotationAxis(&m_, dir.GetConst(), angle);
}

void 
Matrix::Identity()
{
	D3DXMatrixIdentity(&m_);
}

float 
Matrix::Determinent()
{
	return D3DXMatrixDeterminant(&m_);
}

void 
Matrix::Inverse()
{
	Matrix out;

	D3DXMatrixInverse(&out.m_, NULL, &m_);

	*this = out;
}

void 
Matrix::Transpose()
{
	// 같은 행렬을 쓰는게 안전한가?
	Matrix out;

	D3DXMatrixTranspose(&out.m_, &m_);

	*this = out;
}

void 
Matrix::Multiply(const Matrix& m, Matrix& r)
{
	D3DXMatrixMultiply(r.Get(), &m_, m.GetConst());
}

void 
Matrix::Translate(const Vector3& v)
{
	D3DXMatrixTranslation(&m_, v.X(), v.Y(), v.Z());
}

void 
Matrix::Scale(const Vector3& p)
{
	D3DXMatrixScaling(&m_, p.X(), p.Y(), p.Z());
}

void 
Matrix::Scale(const float f)
{
	Scale(Vector3(f, f, f));
}

void 
Matrix::RotateX(float angle)
{
	D3DXMatrixRotationX(&m_, angle);
}

void 
Matrix::RotateY(float angle)
{
	D3DXMatrixRotationY(&m_, angle);
}

void 
Matrix::RotateZ(float angle)
{
	D3DXMatrixRotationZ(&m_, angle);
}

void 
Matrix::LookAtLH(const Vector3& eye, const Vector3& lookAt, const Vector3& up)
{
	D3DXMatrixLookAtLH(&m_, eye.GetConst(), lookAt.GetConst(), up.GetConst());
}

void 
Matrix::PerspectiveFovLH(float angle, float aspect, float nz, float fz)
{
	D3DXMatrixPerspectiveFovLH(&m_, angle, aspect, nz , fz );
}

void 
Matrix::OrthoLH( float width, float height, float nz, float fz )
{
	D3DXMatrixOrthoLH( &m_, width, height, nz, fz );
}
