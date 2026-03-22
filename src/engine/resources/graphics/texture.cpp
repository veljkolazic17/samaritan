#include <engine/resources/graphics/texture.hpp>

#include <engine/graphics/systems/texturesystem.hpp>
#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>

#include <format>
//HACK
#define STB_IMAGE_IMPLEMENTATION
#include <museum/museum.hpp>

BEGIN_NAMESPACE

void Texture::OnLoad()
{
#if SM_USE_MUSEUM_STB

    constexpr smint32 channelCount = 4;

    stbi_set_flip_vertically_on_load(true);

    // If the name already looks like a file path (contains a slash), use it directly.
    // Otherwise search assets/textures/ and try common image extensions.
    std::string path;
    const bool isFullPath = m_Name.find('/') != std::string::npos
                         || m_Name.find('\\') != std::string::npos;

    smuint8* data = nullptr;
    if (isFullPath)
    {
        path = m_Name;
        LogDebug(LogChannel::Resource, "Texture: loading '%s'", path.c_str());
        data = stbi_load(path.data(),
            reinterpret_cast<int*>(&m_Width),
            reinterpret_cast<int*>(&m_Height),
            reinterpret_cast<int*>(&m_ChannelCount),
            channelCount);
        if (!data)
            LogError(LogChannel::Resource, "Texture: stbi failed '%s': %s", path.c_str(), stbi_failure_reason());
    }
    else
    {
        constexpr smcstring extensions[] = { "png", "jpg", "jpeg" };
        for (smcstring ext : extensions)
        {
            path = std::format("assets/textures/{}.{}", m_Name.data(), ext);
            data = stbi_load(path.data(),
                reinterpret_cast<int*>(&m_Width),
                reinterpret_cast<int*>(&m_Height),
                reinterpret_cast<int*>(&m_ChannelCount),
                channelCount);
            if (data) break;
        }
        if (!data)
            LogError(LogChannel::Resource, "Texture: stbi failed for '%s' (tried png/jpg/jpeg)", m_Name.c_str());
    }

    //Generation of default texture
    m_ChannelCount = channelCount;
    if (data != nullptr)
    {
        smuint64 size = m_Width * m_Height * channelCount;

        smbool isTransparent = false;
        for (smuint64 i = 0; i < size; i += channelCount)
        {
            smuint8 alpha = data[i + 3];
            if (alpha < 255)
            {
                isTransparent = true;
                break;
            }
        }

        smRenderer().CreateTexture
        (
            data,
            this
        );

        stbi_image_free(data);
    }
    if (data == nullptr)
    {
        m_State = ResourceState::Error;
    }
    else
    {
        LogDebug(LogChannel::Resource, "Texture: loaded '%s' (%ux%u)", path.c_str(), m_Width, m_Height);
        m_State = ResourceState::Loaded;
    }
#endif
}

void Texture::OnUnload()
{
    smRenderer().DestroyTexture(this);
}

END_NAMESPACE