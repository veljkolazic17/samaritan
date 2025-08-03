#pragma once
#include <defines.hpp>

#include <engine/resources/resource.hpp>

#define SM_DEFAULT_TEXTURE_NAME     "DEFAULT_TEXTURE"
#define SM_TEXTURE_NAME_MAX_LENGTH  1024

BEGIN_NAMESPACE

enum class TextureUsageType
{
    TEXTURE_USAGE_DEFAULT,
    TEXTURE_USAGE_MAP_DIFFUSE,
    TEXTURE_USAGE_UNKNOWN
};

struct Texture : Resource
{
    using Resource::Resource;

    smuint32 m_Width = 0;
    smuint32 m_Height = 0;
    smuint32 m_ChannelCount = 0;
    smbool m_HasTransparency = false;
    void* m_Data = nullptr;

    smuint32 m_Id = SM_INVALID_ID;
    smuint32 m_Generation = SM_INVALID_ID;

    void OnUnload() override;
    void OnLoad() override;
};

struct TextureMap
{
    ResourceHandle<Texture> m_Texture;
    TextureUsageType m_Type = TextureUsageType::TEXTURE_USAGE_UNKNOWN;
};



END_NAMESPACE