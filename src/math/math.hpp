#pragma once
#include <defines.hpp>
#include <type_traits>
#include <cmath>

BEGIN_NAMESPACE

namespace Math
{
	constexpr mfloat32	PI = 3.14159265358979323846f;
	constexpr mfloat32	E = 2.71828182845904523536f;
	constexpr mfloat32	Deg2RadMultiplier = PI / 180.0f;
	constexpr mfloat32	Rad2DegMultiplier = 180.0f / PI;

	SM_INLINE mfloat32 Deg2Rad(mfloat32 a) { return a * Deg2RadMultiplier; }
	SM_INLINE mfloat32 Rad2Deg(mfloat32 a) { return a * Rad2DegMultiplier; }

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
}

END_NAMESPACE