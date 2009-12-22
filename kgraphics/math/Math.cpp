#include "stdafx.h"

#include <kgraphics/math/Math.h>

#include <math.h>

using namespace gfx;

const float Math::PI			= 3.141592f;
const float Math::EPSILON		= 0.00001f;	

float Math::RadianToDegree(float rad)
{
	return (rad*180/PI);
}

float Math::DegreeToRadian(float deg)
{
	return (deg*PI/180);
}

float Math::Sin(float rad)
{
	return ::sin(rad);
}

float Math::Cos(float rad)
{
	return ::cos(rad);
}

float Math::Tan(float rad)
{
	return ::tan(rad);
}

float Math::Sqrt(float f)
{
	return ::sqrt(f);
}

float Math::Abs(float f)
{
	if ( f < 0 )
	{
		return -f;
	}

	return f;
}
