#include <engine/graphics/systems/texturesystem.hpp>

#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>

#include <format>
#include <cstring>

//HACK
#define STB_IMAGE_IMPLEMENTATION
#include <museum/museum.hpp>

BEGIN_NAMESPACE

smbool TextureSystem::Init(const TextureSystemConfing& config)
{
    if (config.m_MaxTextureCount == 0)
    {
        softAssert(false, "Texture System Config has max texture count 0!");
        return false;
    }

    m_Config = config;

    //TODO : [FUCKED][TEXTURE] This is bad. Why? Idk it feels bad check this later
    Texture temp;
    std::fill_n(std::back_inserter(m_Textures), config.m_MaxTextureCount, temp);

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
    
    std::strncpy(reinterpret_cast<char*>(m_DefaultTexture.m_Name), SM_DEFAULT_TEXTURE_NAME, SM_TEXTURE_NAME_MAX_LENGTH);
    m_DefaultTexture.m_HasTransparency = false;
    m_DefaultTexture.m_Generation = SM_INVALID_ID;
    m_DefaultTexture.m_Width = textureDimension;
    m_DefaultTexture.m_Height = textureDimension;
    m_DefaultTexture.m_ChannelCount = 4;

    if (!smRenderer().CreateTexture(pixels, &m_DefaultTexture))
    {
        softAssert(false, "Cannot create default texutre!");
        return false;
    }

    m_DefaultTexture.m_Generation = SM_INVALID_ID;

    return true;
}

void TextureSystem::Shutdown()
{
    Graphics::Renderer& renderer = smRenderer();
    renderer.DestroyTexture(&m_DefaultTexture);
    for (Texture& texture : m_Textures)
    {
        renderer.DestroyTexture(&texture);
    }
}

Texture* TextureSystem::Acquire(const smstring& name, smbool shouldAutoRelease)
{
    if (!std::strcmp(name.data(), SM_DEFAULT_TEXTURE_NAME))
    {
        softAssert(false, "Trying to acquire default texture!");
        return &m_DefaultTexture;
    }

    TextureReference& reference = m_TextureLookup[name];

    if (reference.m_RefCount == 0)
    {
        reference.m_ShouldAutoRelease = shouldAutoRelease;
    }

    ++reference.m_RefCount;
    //First time getting texture
    if (reference.m_Handle == SM_INVALID_ID)
    {
        //Find free space in array
        //TODO : [TEXTURE] change this not to be vector
        Texture* newTexture = nullptr;
        for (smuint64 counter = 0; counter < m_Textures.size(); ++counter)
        {
            Texture& texture = m_Textures[counter];
            if (texture.m_ID == SM_INVALID_ID)
            {
                newTexture = &texture;
                reference.m_Handle = counter;
                break;
            }
        }

        if (newTexture == nullptr)
        {
            softAssert(false, "Can't load more textures!");
            return &m_DefaultTexture;
        }

        if (!LoadTexture(name, newTexture))
        {
            softAssert(false, "Failed to load texture %s", name);
            return &m_DefaultTexture;
        }
        newTexture->m_ID = reference.m_Handle;
    }
    return &m_Textures[reference.m_Handle];
}

void TextureSystem::Release(const smstring& name)
{
    if (!std::strcmp(name.data(), SM_DEFAULT_TEXTURE_NAME))
    {
        softAssert(false, "Trying to release default texture!");
    }

    if (m_TextureLookup.contains(name))
    {
        TextureReference& reference = m_TextureLookup[name];
        --reference.m_RefCount;

        if (reference.m_RefCount == 0 && reference.m_ShouldAutoRelease)
        {
            Texture* texture = &m_Textures[reference.m_Handle];
            smRenderer().DestroyTexture(texture);
            smZero(texture, sizeof(Texture));
            //Texture is destroyed
            texture->m_ID = SM_INVALID_ID;
            texture->m_Generation = SM_INVALID_ID;
            reference.m_Handle = SM_INVALID_ID;
            reference.m_ShouldAutoRelease = false;
        }
        //TODO : [TEXTURE] Check when reference should be removed from entries
    }
    else
    {
        softAssert(false, "Texture not loaded!");
    }
}

smbool TextureSystem::LoadTexture(const smstring& textureName, Texture* texture)
{
#if SM_USE_MUSEUM_STB

    //TODO : [SYSTEM][TEXTURE] Support dynamic file paths
    constexpr smcstring pathFormat = "../../../assets/textures/{}.{}";
    constexpr smint32 channelCount = 4;

    stbi_set_flip_vertically_on_load(true);
    char full_file_path[512];

    //TODO : [SYSTEM][TEXUTRE] Support multiple formats
    std::string path = std::format(pathFormat, textureName.data(), "png");

    Texture temp;
    smuint8* data = stbi_load
    (
        path.data(),
        reinterpret_cast<int*>(&temp.m_Width),
        reinterpret_cast<int*>(&temp.m_Height),
        reinterpret_cast<int*>(&temp.m_ChannelCount),
        channelCount
    );

    //Generation of default texture
    temp.m_ChannelCount = channelCount;
    if (data != nullptr)
    {
        smuint32 generation = texture->m_Generation;
        texture->m_Generation = SM_INVALID_ID;

        smuint64 size = temp.m_Width * temp.m_Height * channelCount;

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

        // Acquire internal texture resources and upload to GPU.
        Graphics::Renderer& renderer = smRenderer();

        //TODO : [CRITICAL] check this shit code later!!!!
        std::strncpy(reinterpret_cast<char*>(temp.m_Name), textureName.data(), SM_TEXTURE_NAME_MAX_LENGTH);
        renderer.CreateTexture
        (
            data,
            &temp
        );

        Texture oldTexture = *texture;
        *texture = temp;
        renderer.DestroyTexture(&oldTexture);

        //Check generation
        if (generation == SM_INVALID_ID)
        {
            texture->m_Generation = 0;
        }
        else
        {
            texture->m_Generation = generation + 1;
        }

        stbi_image_free(data);
        return true;
    }
    if (const char* reason = stbi_failure_reason())
    {
        //TODO [TEXTURE] Add support for asserts messages {} or %s
        softAssert(false, "Failed to load texture", path.data(), reason);
        stbi__err(0, 0);
    }
#endif
    return false;
};

END_NAMESPACE
