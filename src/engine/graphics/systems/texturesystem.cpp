#include <engine/graphics/systems/texturesystem.hpp>

#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>

#include <engine/resources/resourcesystem.hpp>

#include <format>
#include <cstring>

BEGIN_NAMESPACE

smbool TextureSystem::Init()
{
    //Create default texture
    constexpr smuint64 textureDimension = 256;
    constexpr smuint64 channels = 4;
    constexpr smuint64 pixelCount = textureDimension * textureDimension;

    constexpr smuint64 textureSize = pixelCount * channels;
    smuint8 pixels[textureSize];
    smSet(pixels, 255, sizeof(smuint8) * textureSize);

    for (smuint64 row = 0; row < textureDimension; ++row)
    {
        bool rowIsOdd = row % 2; // Precompute row parity
        for (smuint64 col = 0; col < textureDimension; ++col)
        {
            if (col % 2 == rowIsOdd) // Check if row and column parity match
            {
                smuint64 index_bpp = (row * textureDimension + col) * channels;
                pixels[index_bpp + 0] = 0;
                pixels[index_bpp + 1] = 0;
            }
        }
    }
    
    m_DefaultTextureData.m_Name = std::string(SM_DEFAULT_TEXTURE_NAME);
    m_DefaultTextureData.m_HasTransparency = false;
    m_DefaultTextureData.m_Width = textureDimension;
    m_DefaultTextureData.m_Height = textureDimension;
    m_DefaultTextureData.m_ChannelCount = 4;

    if (!smRenderer().CreateTexture(pixels, &m_DefaultTextureData))
    {
        softAssert(false, "Cannot create default texutre!");
        return false;
    }

    constexpr smbool shouldAutoRelease = false;

    m_DefaultTexture = smResourceSystem().Load<Texture>(&m_DefaultTextureData, shouldAutoRelease);
}

void TextureSystem::Shutdown()
{
    smRenderer().DestroyTexture(&m_DefaultTextureData);
    m_TextureLookup.clear();
}

void TextureSystem::SingletonInit()
{
    Init();
}

void TextureSystem::SingletonShutdown()
{
    Shutdown();
}

const ResourceHandle<Texture>& TextureSystem::Acquire(const smstring& name, smbool shouldAutoRelease)
{
    if (!std::strcmp(name.data(), SM_DEFAULT_TEXTURE_NAME))
    {
        softAssert(false, "TextureSystem: Trying to acquire default texture!");
        return ResourceHandle<Texture>::InvalidReference();
    }

    ResourceHandle<Texture>& texture = m_TextureLookup[name];

    if (!texture.IsValid())
    {
        texture = std::move(smResourceSystem().Load<Texture>(name, shouldAutoRelease));
        if (!texture.IsValid())
        {
            softAssert(false, "TextureSystem: Could not load texture %s", name);
            return ResourceHandle<Texture>::InvalidReference();
        }
    }
    return texture;
}

void TextureSystem::Release(const smstring& name)
{
    if (!std::strcmp(name.data(), SM_DEFAULT_TEXTURE_NAME))
    {
        softAssert(false, "TextureSystem: Trying to release default texture!");
    }

    if (m_TextureLookup.contains(name))
    {
        m_TextureLookup.erase(name);
    }
    else
    {
        softAssert(false, "Texture not loaded!");
    }
}

END_NAMESPACE
