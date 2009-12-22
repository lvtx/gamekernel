#include "stdafx.h"

#include <kgraphics/math/Quaternion.h>
#include <kgraphics/math/Matrix.h>

using namespace gfx;

Quaternion::Quaternion()
{
	Identity();
}

Quaternion::Quaternion(const Vector3& dir, float angle)
{
	D3DXQuaternionRotationAxis(&q_, dir.GetConst(), angle);
}

Quaternion::Quaternion(const Matrix& m)
{
	D3DXQuaternionRotationMatrix(&q_, m.GetConst());
}

Quaternion::Quaternion(float pitch, float yaw, float roll)
{
	D3DXQuaternionRotationYawPitchRoll(&q_, yaw, pitch, roll);
}

Quaternion::Quaternion(const Quaternion& rhs)
{
	q_.x = rhs.X();
	q_.y = rhs.Y();
	q_.z = rhs.Z();
	q_.w = rhs.W();
}

Quaternion& 
Quaternion::operator=(const Quaternion& rhs)
{
	q_.x = rhs.X();
	q_.y = rhs.Y();
	q_.z = rhs.Z();
	q_.w = rhs.W();

	return *this;
}

Quaternion 
Quaternion::Slerp(const Quaternion& maxRotation, float factor) const
{
	Quaternion out;

	D3DXQuaternionSlerp(out.Get(), &q_, maxRotation.GetConst(), factor);

	return out;
}

/// this 곱하기 q. 나로 회전하고 다시 q만큼 회전한 쿼터니언
Quaternion 
Quaternion::Multiply(const Quaternion& q) const
{
	Quaternion out;

	D3DXQuaternionMultiply(out.Get(), &q_, q.GetConst());

	return out;
}

void 
Quaternion::Inverse()
{
	Quaternion out;

	D3DXQuaternionInverse(out.Get(), &q_);

	*this = out;
}

void 
Quaternion::Identity()
{
	D3DXQuaternionIdentity(&q_);
}


bool 
Quaternion::IsIdentity() const
{
	return (LengthSq() == 1);
}

float 
Quaternion::Length() const
{
	return D3DXQuaternionLength(&q_);
}

float 
Quaternion::LengthSq() const
{
	return D3DXQuaternionLengthSq(&q_);
}

float 
Quaternion::Dot(const Quaternion& q) const
{
	return D3DXQuaternionDot(&q_, q.GetConst());
}

Quaternion 
Quaternion::Conjugate() const
{
	Quaternion out;

	D3DXQuaternionConjugate(out.Get(), &q_);

	return out;
}

void 
Quaternion::GetAxisAngle(Vector3& dir, float& angle) const
{
	D3DXQuaternionToAxisAngle(&q_, dir.Get(), &angle);
}

float 
Quaternion::X() const
{
	return q_.x;
}

float 
Quaternion::Y() const
{
	return q_.y;
}

float 
Quaternion::Z() const
{
	return q_.z;
}

float 
Quaternion::W() const
{
	return q_.w;
}