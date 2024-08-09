#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

namespace Math
{
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
}

END_NAMESPACE