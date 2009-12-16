#pragma once 

#include "Math.h"

#include <d3dx9.h>

namespace gfx {

class Matrix;
class Vector4;

class Vector2 
{
public:
	Vector2();

	Vector2(float x, float y);

	Vector2(const Vector2& rhs);

	Vector2& operator=(const Vector2& rhs);

    D3DXVECTOR2* Get()
	{
		return &v_;
	}

	const D3DXVECTOR2* GetConst() const
	{
		return &v_;
	}

	/// p1을 자기에 더해서 자기를 돌려준다.
	const Vector2& Add(const Vector2& p1);

	/// this x p를 돌려준다. 외적. z값을 돌려준다.
	float Cross(const Vector2& p);

	/// 내적
	float Dot(const Vector2& p);

	/// 길이
	float Length();

	/// 길이의 제곱값
	float LengthSq();

	/// 현재 값으 ㄹ시작점으로 다른 벡터를 돌려준다.
	Vector2 Lerp(const Vector2& endPoint, float factor);

	/// 최대값들로 이루어진 벡터
	void Maximize(const Vector2& p);

	/// 최소값들로 이루어진 벡터
	void Minimize(const Vector2& p);

	/// 단위 벡터로 만듬
	void Normalize();

	/// 확대 
	void Scale(float s);

	/// 빼기
	void Subtract(const Vector2& p);

	/// 자기 (x, y, 0, 1)을 m으로 변환한 결과
	Vector4 Transform(const Matrix& m);

	/// accessor
	float X() const;

	/// accessor
	float Y() const;

	void IncX( float delta ); 
	void IncY( float delta ); 

public:
	D3DXVECTOR2 v_;

};

class Vector3
{
public:
	Vector3();
	Vector3(float x, float y, float z);

	D3DXVECTOR3* Get()
	{
		return &v_;
	}

	const D3DXVECTOR3* GetConst() const
	{
		return &v_;
	}

	Vector3(const Vector3& rhs);

	Vector3& operator=(const Vector3& rhs);

	/// p1을 자기에 더해서 자기를 돌려준다.
	const Vector3& Add(const Vector3& p1);

	/// this x p를 돌려준다. 외적. z값을 돌려준다.
	Vector3 Cross(const Vector3& p);

	/// 내적
	float Dot(const Vector3& p);

	/// 길이
	float Length();

	/// 길이의 제곱값
	float LengthSq();

	/// 현재 값으 ㄹ시작점으로 다른 벡터를 돌려준다.
	Vector3 Lerp(const Vector3& endPoint, float factor);

	/// 최대값들로 이루어진 벡터
	void Maximize(const Vector3& p);

	/// 최소값들로 이루어진 벡터
	void Minimize(const Vector3& p);

	/// 단위 벡터로 만듬
	void Normalize();

	/// 확대 
	void Scale(float s);

	/// 빼기
	void Subtract(const Vector3& p);

	/// 자기 (x, y, z, 1)을 m으로 변환한 결과
	Vector4 Transform(const Matrix& m);

	/// accessor
	float X() const;

	/// accessor
	float Y() const;

	/// accessor
	float Z() const;

	void IncX( float delta ); 
	void IncY( float delta ); 
	void IncZ( float delta ); 

public:
	D3DXVECTOR3 v_;
};

class Vector4
{
public:
	Vector4();
	Vector4(float x, float y, float z, float w);

	D3DXVECTOR4* Get()
	{
		return &v_;
	}

	const D3DXVECTOR4* GetConst() const
	{
		return &v_;
	}

	Vector4(const Vector4& rhs);

	Vector4& operator=(const Vector4& rhs);

	/// p1을 자기에 더해서 자기를 돌려준다.
	const Vector4& Add(const Vector4& p1);

	/// 4차원 외적. 의미는 무엇인가?
	Vector4 Cross(const Vector4& p, const Vector4& p2);

	/// 내적
	float Dot(const Vector4& p);

	/// 길이
	float Length();

	/// 길이의 제곱값
	float LengthSq();

	/// 현재 값으 ㄹ시작점으로 다른 벡터를 돌려준다.
	Vector4 Lerp(const Vector4& endPoint, float factor);

	/// 최대값들로 이루어진 벡터
	void Maximize(const Vector4& p);

	/// 최소값들로 이루어진 벡터
	void Minimize(const Vector4& p);

	/// 단위 벡터로 만듬
	void Normalize();

	/// 확대 
	void Scale(float s);

	/// 빼기
	void Subtract(const Vector4& p);

	/// 자기 (x, y, z, w)을 m으로 변환한 결과
	Vector4 Transform(const Matrix& m);

	/// accessor
	float X() const;

	/// accessor
	float Y() const;

	/// accessor
	float Z() const;

	/// accessor
	float W() const;

	void IncX( float delta ); 
	void IncY( float delta ); 
	void IncZ( float delta ); 

public:
	D3DXVECTOR4 v_;
};

} // namespace gfx


