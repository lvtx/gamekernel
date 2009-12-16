#pragma once

namespace gfx
{
/** 
 * @class Math
 *
 * @brief 수학 함수들을 여기에 둔다. 나중에 최적화 여지를 주기위해 한곳에 모은다. 
 */
class Math 
{
public:
	static const float PI;
	static const float EPSILON;

	static float RadianToDegree(float rad);
	static float DegreeToRadian(float deg);

	static float Sin(float rad);
	static float Cos(float rad);
	static float Tan(float rad);

	static float Sqrt(float f);
	static float Abs(float f);

private:
	static double invSqrt(double x);
};

} // namespace gfx

