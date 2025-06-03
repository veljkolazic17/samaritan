#pragma once
#include <cassert>
#include <defines.hpp>
#include <type_traits>
#include <cmath>

BEGIN_NAMESPACE

namespace Math
{
	constexpr smfloat32			PI = 3.14159265358979323846f;
	constexpr smfloat32			E = 2.71828182845904523536f;
	constexpr smfloat32			Deg2RadMultiplier = PI / 180.0f;
	constexpr smfloat32			Rad2DegMultiplier = 180.0f / PI;

	SM_INLINE smfloat32			Deg2Rad(smfloat32 a) { return a * Deg2RadMultiplier; }
	SM_INLINE smfloat32			Rad2Deg(smfloat32 a) { return a * Rad2DegMultiplier; }

	#define smVec3_up			(smVec3) { 0.0f, 1.0f, 0.0f }
	#define smVec3_down			(smVec3) { 0.0f, -1.0f, 0.0f }
	#define smVec3_left			(smVec3) { -1.0f, 0.0f, 0.0f }
	#define smVec3_right		(smVec3) { 1.0f, 0.0f, 0.0f }
	#define smVec3_forward		(smVec3) { 0.0f, 0.0f, -1.0f }
	#define smVec3_back			(smVec3) { 0.0f, 0.0f, 1.0f }

	SM_INLINE smint32 Abs(smint32 x)
	{
		int y = x >> 31;
		return ((x ^ y) - y);
	}

	SM_INLINE smfloat32 Fabs(smfloat32 f) 
	{
		int tmp = *reinterpret_cast<int*>(&f);
		tmp &= 0x7FFFFFFF;
		return *reinterpret_cast<float*>(&tmp);
	}

	SM_INLINE smint8 ClampChar(smint8 i) 
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

	SM_INLINE smint16 ClampShort(smint16 i) 
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

	SM_INLINE smint32 ClampInt(smint32 min, smint32 max, smint32 value) 
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

	SM_INLINE smfloat32 ClampFloat(smfloat32 min, smfloat32 max, smfloat32 value) 
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

	SM_INLINE smfloat32 Tan(smfloat32 a) 
	{
		return std::tanf(a);
	}

	SM_INLINE smfloat32 Sin(smfloat32 a)
	{
		return std::sinf(a);
	}

	SM_INLINE smfloat32 Cos(smfloat32 a)
	{
		return std::cosf(a);
	}

	SM_INLINE smfloat32 DOOM_InvSqrt(smfloat32 a)
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