#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

namespace TypeUtils
{
    // Example usage of GetAlignedVAlue:
    // value = 13, alignment = 8
    // Expected result: 16
    // Calculation: ((13 + (8 - 1)) & ~(8 - 1)) = (20 & ~7) = (20 & 0xFFFFFFF8) = 16

    // value = 25, alignment = 16
    // Expected result: 32
    // Calculation: ((25 + (16 - 1)) & ~(16 - 1)) = (40 & ~15) = (40 & 0xFFFFFFF0) = 32

    // value = 64, alignment = 32
    // Expected result: 64
    // Calculation: ((64 + (32 - 1)) & ~(32 - 1)) = (95 & ~31) = (95 & 0xFFFFFFE0) = 64
    SM_INLINE smuint64 GetAlignedValue(smuint64 value, smuint64 alignment)
    {
        return ((value + (alignment - 1)) & ~(alignment - 1));
    }

    SM_INLINE Range GetAllignedRange(smuint32 offset, smuint32 size, smuint32 alligment)
    {
        Range range;
        range.m_Offset = GetAlignedValue(offset, alligment);
        range.m_Size = GetAlignedValue(size, alligment);
        return range;
    }
}

END_NAMESPACE