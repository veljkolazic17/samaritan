#pragma once
#include <defines.hpp>

#define SM_DEFAULT_TEXTURE_NAME     "DEFAULT"
#define SM_TEXTURE_NAME_MAX_LENGTH  1024

BEGIN_NAMESPACE

class Texture;

enum class TextureUsageType
{
    TEXTURE_USAGE_DEFAULT,
    TEXTURE_USAGE_MAP_DIFFUSE,
    TEXTURE_USAGE_UNKNOWN
};

struct TextureMap
{
    Texture* m_Texture = nullptr;
    TextureUsageType m_Type = TextureUsageType::TEXTURE_USAGE_UNKNOWN;
};

struct Texture
{
    smuint8 m_Name[SM_TEXTURE_NAME_MAX_LENGTH];
    smuint32 m_ID = SM_INVALID_ID;
    smuint32 m_Width = 0;
    smuint32 m_Height = 0;
    smuint32 m_ChannelCount = 0;
    smbool m_HasTransparency = false;
    smuint32 m_Generation = SM_INVALID_ID;
    void* m_Data = nullptr;
};

END_NAMESPACE