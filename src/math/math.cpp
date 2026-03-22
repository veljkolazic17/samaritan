#include <math/math.hpp>
#include <math/vector.hpp>

BEGIN_NAMESPACE

namespace Math
{
    void ComputeTangentBitangent
    (
        const smVec3& p0, const smVec3& p1, const smVec3& p2,
        const smVec2& uv0, const smVec2& uv1, const smVec2& uv2,
        smVec3& outTangent, smVec3& outBitangent
    )
    {
        const smfloat32 e1x = p1.m_X - p0.m_X, e1y = p1.m_Y - p0.m_Y, e1z = p1.m_Z - p0.m_Z;
        const smfloat32 e2x = p2.m_X - p0.m_X, e2y = p2.m_Y - p0.m_Y, e2z = p2.m_Z - p0.m_Z;
        const smfloat32 du1 = uv1.m_X - uv0.m_X, dv1 = uv1.m_Y - uv0.m_Y;
        const smfloat32 du2 = uv2.m_X - uv0.m_X, dv2 = uv2.m_Y - uv0.m_Y;

        const smfloat32 det = du1 * dv2 - du2 * dv1;
        const smfloat32 f = (det != 0.0f) ? 1.0f / det : 0.0f;

        outTangent   = smVec3(f * (dv2 * e1x - dv1 * e2x), f * (dv2 * e1y - dv1 * e2y), f * (dv2 * e1z - dv1 * e2z));
        outBitangent = smVec3(f * (-du2 * e1x + du1 * e2x), f * (-du2 * e1y + du1 * e2y), f * (-du2 * e1z + du1 * e2z));
    }
}

END_NAMESPACE