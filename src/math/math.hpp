#pragma once
#include <cassert>
#include <defines.hpp>
#include <type_traits>
#include <cmath>

BEGIN_NAMESPACE

namespace Math
{
	constexpr mfloat32			PI = 3.14159265358979323846f;
	constexpr mfloat32			E = 2.71828182845904523536f;
	constexpr mfloat32			Deg2RadMultiplier = PI / 180.0f;
	constexpr mfloat32			Rad2DegMultiplier = 180.0f / PI;

	SM_INLINE mfloat32			Deg2Rad(mfloat32 a) { return a * Deg2RadMultiplier; }
	SM_INLINE mfloat32			Rad2Deg(mfloat32 a) { return a * Rad2DegMultiplier; }

	#define smVec3_up			(smVec3) { 0.0f, 1.0f, 0.0f }
	#define smVec3_down			(smVec3) { 0.0f, -1.0f, 0.0f }
	#define smVec3_left			(smVec3) { -1.0f, 0.0f, 0.0f }
	#define smVec3_right		(smVec3) { 1.0f, 0.0f, 0.0f }
	#define smVec3_forward		(smVec3) { 0.0f, 0.0f, -1.0f }
	#define smVec3_back			(smVec3) { 0.0f, 0.0f, 1.0f }

	SM_INLINE mint32 Abs(mint32 x)
	{
		int y = x >> 31;
		return ((x ^ y) - y);
	}

	SM_INLINE mfloat32 Fabs(mfloat32 f) 
	{
		int tmp = *reinterpret_cast<int*>(&f);
		tmp &= 0x7FFFFFFF;
		return *reinterpret_cast<float*>(&tmp);
	}

	SM_INLINE mint8 ClampChar(mint8 i) 
	{
		if (i < -128) 
		{
			return -128;
		}
		if (i > 127) 
		{
			return 127;
		}
		return i;
	}

	SM_INLINE mint16 ClampShort(mint16 i) 
	{
		if (i < -32768) 
		{
			return -32768;
		}
		if (i > 32767) 
		{
			return 32767;
		}
		return i;
	}

	SM_INLINE mint32 ClampInt(mint32 min, mint32 max, mint32 value) 
	{
		if (value < min) 
		{
			return min;
		}
		if (value > max) 
		{
			return max;
		}
		return value;
	}

	SM_INLINE mfloat32 ClampFloat(mfloat32 min, mfloat32 max, mfloat32 value) 
	{
		if (value < min) 
		{
			return min;
		}
		if (value > max) 
		{
			return max;
		}
		return value;
	}

	SM_INLINE mfloat32 Tan(mfloat32 a) 
	{
		return std::tanf(a);
	}

	SM_INLINE mfloat32 Sin(mfloat32 a)
	{
		return std::sinf(a);
	}

	SM_INLINE mfloat32 Cos(mfloat32 a)
	{
		return std::cosf(a);
	}

	SM_INLINE mfloat32 DOOM_InvSqrt(mfloat32 a)
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = a * 0.5F;
		y = a;
		i = *(long*)&y;
		i = 0x5f3759df - (i >> 1);
		y = *(float*)&i;
		y = y * (threehalfs - (x2 * y * y));

		return y;
	}
}

END_NAMESPACE