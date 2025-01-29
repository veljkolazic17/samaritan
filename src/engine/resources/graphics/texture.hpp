#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

struct Texture
{
    muint32 m_ID;
    muint32 m_Width;
    muint32 m_Height;
    muint32 m_ChannelCount;
    mbool m_HasTransparency;
    muint32 m_Generation;
    void* m_Data;
};

END_NAMESPACE