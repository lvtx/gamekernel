#pragma once 

#include "Vector.h"

namespace gfx
{
class Matrix;

class Quaternion 
{
public:
	/// 단위 쿼터니언
	Quaternion();

	/// dir을 축으로 angle만큼 회전하는 쿼터니언
	Quaternion(const Vector3& dir, float angle);

	/// 회전 행렬로부터 Quaternion 생성
	Quaternion(const Matrix& m);

	/// x, y, z 축 회전 값으로 생성
	Quaternion(float pitch, float yaw, float roll);

	/// 복사 생성자
	Quaternion(const Quaternion& rhs);

	/// 할당 연산자
	Quaternion& operator=(const Quaternion& rhs);

	D3DXQUATERNION* Get() 
	{
		return &q_;
	}

	const D3DXQUATERNION* GetConst() const
	{
		return &q_;
	}

	/// slerp using this as a starting rotation
	Quaternion Slerp(const Quaternion& maxRotation, float factor) const;

	/// this 곱하기 q. 나로 회전하고 다시 q만큼 회전한 쿼터니언
	Quaternion Multiply(const Quaternion& q) const;
	/// 
	void Inverse();		

	void Identity();

	void Normalize();

	bool IsIdentity() const;

	float Length() const;

	float LengthSq() const;

	float Dot(const Quaternion& q) const;

	Quaternion Conjugate() const;

	/// 쿼터니언에서 축과 회전값을 가져옴
	void GetAxisAngle(Vector3& dir, float& angle) const;

	float X() const;

	float Y() const;

	float Z() const;

	float W() const;

public:
	D3DXQUATERNION q_;
};

} // namespace gfx

