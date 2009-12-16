#include "stdafx.h"

#include "Vector.h"

#include "Matrix.h"

using namespace gfx;

Vector2::Vector2()
{
	v_.x = 0;
	v_.y = 0;
}

Vector2::Vector2(float x, float y)
{
	v_.x = x;
	v_.y = y;
}

Vector2::Vector2(const Vector2& rhs)
{
	v_.x = rhs.X();
	v_.y = rhs.Y();
}

Vector2& 
Vector2::operator=(const Vector2& rhs)
{
	v_.x = rhs.X();
	v_.y = rhs.Y();

	return *this;
}

const Vector2& 
Vector2::Add(const Vector2& p1)
{
	D3DXVec2Add(&v_, p1.GetConst(), &v_);

	return *this;
}

float 
Vector2::Cross(const Vector2& p)
{
	return D3DXVec2CCW(&v_, p.GetConst());
}

float 
Vector2::Dot(const Vector2& p)
{
	return D3DXVec2Dot(&v_, p.GetConst());
}

float 
Vector2::Length()
{
	return D3DXVec2Length(&v_);
}

float 
Vector2::LengthSq()
{
	return D3DXVec2LengthSq(&v_);
}

Vector2 
Vector2::Lerp(const Vector2& endPoint, float factor)
{
	Vector2 out;

	D3DXVec2Lerp(out.Get(), &v_, endPoint.GetConst(), factor);

	return out;
}

void 
Vector2::Maximize(const Vector2& p)
{
	D3DXVec2Maximize(&v_, &v_, p.GetConst());
}

void 
Vector2::Minimize(const Vector2& p)
{
	D3DXVec2Minimize(&v_, &v_, p.GetConst());
}

void 
Vector2::Normalize()
{
	D3DXVec2Normalize(&v_, &v_);
}

void 
Vector2::Scale(float s)
{
	D3DXVec2Scale(&v_, &v_, s);
}

void 
Vector2::Subtract(const Vector2& p)
{
	D3DXVec2Subtract(&v_, &v_, p.GetConst());
}

Vector4 
Vector2::Transform(const Matrix& m)
{
	Vector4 out;

	D3DXVec2Transform(out.Get(), &v_, m.GetConst());

	return out;
}

float 
Vector2::X() const
{
	return v_.x;
}

float 
Vector2::Y() const
{
	return v_.y;
}

void 
Vector2::IncX( float delta )
{
	v_.x += delta;
}

void 
Vector2::IncY( float delta )
{
	v_.y += delta;
}

//------------------------------------ 
// Vector3

Vector3::Vector3()
{
	v_.x = 0;
	v_.y = 0;
	v_.z = 0;
}

Vector3::Vector3(float x, float y, float z)
{
	v_.x = x;
	v_.y = y;
	v_.z = z;
}

Vector3::Vector3(const Vector3& rhs)
{
	v_.x = rhs.X();
	v_.y = rhs.Y();
	v_.z = rhs.Z();
}

Vector3& 
Vector3::operator=(const Vector3& rhs)
{
	v_.x = rhs.X();
	v_.y = rhs.Y();
	v_.z = rhs.Z();

	return *this;
}

const Vector3& 
Vector3::Add(const Vector3& p1)
{
	D3DXVec3Add(&v_, p1.GetConst(), &v_);

	return *this;
}

Vector3 
Vector3::Cross(const Vector3& p)
{
	Vector3 out;

	D3DXVec3Cross(out.Get(), &v_, p.GetConst());

	return out;
}

float 
Vector3::Dot(const Vector3& p)
{
	return D3DXVec3Dot(&v_, p.GetConst());
}

float 
Vector3::Length()
{
	return D3DXVec3Length(&v_);
}

float 
Vector3::LengthSq()
{
	return D3DXVec3LengthSq(&v_);
}

Vector3 
Vector3::Lerp(const Vector3& endPoint, float factor)
{
	Vector3 out;

	D3DXVec3Lerp(out.Get(), &v_, endPoint.GetConst(), factor);

	return out;
}

void 
Vector3::Maximize(const Vector3& p)
{
	D3DXVec3Maximize(&v_, &v_, p.GetConst());
}

void 
Vector3::Minimize(const Vector3& p)
{
	D3DXVec3Minimize(&v_, &v_, p.GetConst());
}

void 
Vector3::Normalize()
{
	D3DXVec3Normalize(&v_, &v_);
}

void 
Vector3::Scale(float s)
{
	D3DXVec3Scale(&v_, &v_, s);
}

void 
Vector3::Subtract(const Vector3& p)
{
	D3DXVec3Subtract(&v_, &v_, p.GetConst());
}

Vector4 
Vector3::Transform(const Matrix& m)
{
	Vector4 out;

	D3DXVec3Transform(out.Get(), &v_, m.GetConst());

	return out;
}

float 
Vector3::X() const
{
	return v_.x;
}

float 
Vector3::Y() const
{
	return v_.y;
}

float 
Vector3::Z() const
{
	return v_.z;
}

void 
Vector3::IncX( float delta )
{
	v_.x += delta;
}

void 
Vector3::IncY( float delta )
{
	v_.y += delta;
}

void 
Vector3::IncZ( float delta )
{
	v_.z += delta;
}

//------------------------------------ 
// Vector4

Vector4::Vector4()
{
	v_.x = 0;
	v_.y = 0;
	v_.z = 0;
	v_.w = 0;
}

Vector4::Vector4(float x, float y, float z, float w)
{
	v_.x = x;
	v_.y = y;
	v_.z = z;
	v_.w = w;
}

Vector4::Vector4(const Vector4& rhs)
{
	v_.x = rhs.X();
	v_.y = rhs.Y();
	v_.z = rhs.Z();
	v_.w = rhs.W();
}

Vector4& Vector4::operator=(const Vector4& rhs)
{
	v_.x = rhs.X();
	v_.y = rhs.Y();
	v_.z = rhs.Z();
	v_.w = rhs.W();

	return *this;
}

const Vector4& 
Vector4::Add(const Vector4& p1)
{
	D3DXVec4Add(&v_, p1.GetConst(), &v_);

	return *this;
}

Vector4 
Vector4::Cross(const Vector4& p, const Vector4& p2)
{
	Vector4 out;

	D3DXVec4Cross(out.Get(), &v_, p.GetConst(), p2.GetConst());

	return out;
}

float 
Vector4::Dot(const Vector4& p)
{
	return D3DXVec4Dot(&v_, p.GetConst());
}

float 
Vector4::Length()
{
	return D3DXVec4Length(&v_);
}

float 
Vector4::LengthSq()
{
	return D3DXVec4LengthSq(&v_);
}

Vector4 
Vector4::Lerp(const Vector4& endPoint, float factor)
{
	Vector4 out;

	D3DXVec4Lerp(out.Get(), &v_, endPoint.GetConst(), factor);

	return out;
}

void 
Vector4::Maximize(const Vector4& p)
{
	D3DXVec4Maximize(&v_, &v_, p.GetConst());
}

void 
Vector4::Minimize(const Vector4& p)
{
	D3DXVec4Minimize(&v_, &v_, p.GetConst());
}

void 
Vector4::Normalize()
{
	D3DXVec4Normalize(&v_, &v_);
}

void 
Vector4::Scale(float s)
{
	D3DXVec4Scale(&v_, &v_, s);
}

void 
Vector4::Subtract(const Vector4& p)
{
	D3DXVec4Subtract(&v_, &v_, p.GetConst());
}

Vector4 
Vector4::Transform(const Matrix& m)
{
	Vector4 out;

	D3DXVec4Transform(out.Get(), &v_, m.GetConst());

	return out;
}

float 
Vector4::X() const
{
	return v_.x;
}

float 
Vector4::Y() const
{
	return v_.y;
}

float 
Vector4::Z() const
{
	return v_.z;
}

float 
Vector4::W() const
{
	return v_.w;
}

void 
Vector4::IncX( float delta )
{
	v_.x += delta;
}

void 
Vector4::IncY( float delta )
{
	v_.y += delta;
}

void 
Vector4::IncZ( float delta )
{
	v_.z += delta;
}