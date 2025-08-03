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

    //TODO : [SYSTEM][TEXTURE] Support dynamic file paths
    constexpr smcstring pathFormat = "../../../assets/textures/{}.{}";
    constexpr smint32 channelCount = 4;

    stbi_set_flip_vertically_on_load(true);
    char full_file_path[512];

    //TODO : [SYSTEM][TEXUTRE] Support multiple formats
    std::string path = std::format(pathFormat, m_Name.data(), "png");

    smuint8* data = stbi_load
    (
        path.data(),
        reinterpret_cast<int*>(&m_Width),
        reinterpret_cast<int*>(&m_Height),
        reinterpret_cast<int*>(&m_ChannelCount),
        channelCount
    );

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
    if (const char* reason = stbi_failure_reason())
    {
        //TODO [TEXTURE] Add support for asserts messages {} or %s
        softAssert(false, "Failed to load texture", path.data(), reason);
        stbi__err(0, 0);
    }
#endif
}

void Texture::OnUnload()
{
    smRenderer().DestroyTexture(this);
}

END_NAMESPACE