#pragma once
#include <defines.hpp>

#define SM_DEFAULT_TEXTURE_NAME     "DEFAULT"
#define SM_TEXTURE_NAME_MAX_LENGTH  1024

BEGIN_NAMESPACE

enum class TextureUsageType
{
    TEXTURE_USAGE_DEFAULT,
    TEXTURE_USAGE_MAP_DIFFUSE,
    TEXTURE_USAGE_UNKNOWN
};

struct TextureMap
{
    Texture* m_Texture = nullptr;
    TextureUsageType m_Type;
};

struct Texture
{
    muint8 m_Name[SM_TEXTURE_NAME_MAX_LENGTH];
    muint32 m_ID = SM_INVALID_ID;
    muint32 m_Width = 0;
    muint32 m_Height = 0;
    muint32 m_ChannelCount = 0;
    mbool m_HasTransparency = false;
    muint32 m_Generation = SM_INVALID_ID;
    void* m_Data = nullptr;
};

END_NAMESPACE