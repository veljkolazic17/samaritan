#pragma once
#include <defines.hpp>

#define SM_DEFAULT_TEXTURE_NAME		"DEFAULT"

BEGIN_NAMESPACE

struct Texture
{
    muint32 m_ID = SM_INVALID_ID;
    muint32 m_Width = 0;
    muint32 m_Height = 0;
    muint32 m_ChannelCount = 0;
    mbool m_HasTransparency = false;
    muint32 m_Generation = SM_INVALID_ID;
    void* m_Data = nullptr;
};

END_NAMESPACE