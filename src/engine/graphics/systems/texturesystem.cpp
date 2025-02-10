#include <engine/graphics/systems/texturesystem.hpp>

#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>

#include <museum/museum.hpp>
#include <format>

BEGIN_NAMESPACE

mbool TextureSystem::Init(const TextureSystemConfing& config)
{
	if (config.m_MaxTextureCount == 0)
	{
		softAssert(false, "Texture System Config has max texture count 0!");
		return false;
	}

	m_Config = config;

	constexpr muint64 textureDimension = 256;
	constexpr muint64 channels = 4;
	constexpr muint64 pixelCount = textureDimension * textureDimension;

	constexpr muint64 textureSize = pixelCount * channels;
	muint8 pixels[textureSize];
	smSet(pixels, 255, sizeof(muint8) * textureSize);

	for (muint64 row = 0; row < textureDimension; ++row)
	{
		bool rowIsOdd = row % 2; // Precompute row parity
		for (muint64 col = 0; col < textureDimension; ++col)
		{
			if (col % 2 == rowIsOdd) // Check if row and column parity match
			{
				muint64 index_bpp = (row * textureDimension + col) * channels;
				pixels[index_bpp + 0] = 0;
				pixels[index_bpp + 1] = 0;
			}
		}
	}

	if (Graphics::Renderer* renderer = smRenderer())
	{
		renderer->CreateTexture
		(
				SM_DEFAULT_TEXTURE_NAME,
				false,
				textureDimension,
				textureDimension,
				channels,
				pixels,
				false,
				&m_DefaultTexture
		);
	}
	else
	{
		hardAssert(false, "Renderer is nullptr!");
	}

	m_DefaultTexture.m_Generation = SM_INVALID_ID;

	return true;
}

void TextureSystem::Shutdown()
{
	Graphics::Renderer* renderer = smRenderer()

	if (renderer == nullptr)
	{
		hardAssert(false, "Renderer is nullptr!");
	}

	renderer->DestroyTexture(&m_DefaultTexture);

	for (const Texture& texture : m_Textures)
	{
		renderer->DestroyTexture(&texture);
	}
}

Texture& TextureSystem::Acquire(mcstring name, mbool shouldAutoRelease)
{


	return nullptr;
}

void TextureSystem::Release(mcstring name)
{

}

mbool LoadTexture(mcstring textureName, Texture* texture)
{
#if SM_USE_MUSEUM_STB

    constexpr mcstring pathFormat = "assets/textures/{}.{}";
    constexpr mint32 channelCount = 4;

    stbi_set_flip_vertically_on_load(true);
    char full_file_path[512];

    //TODO : [SYSTEM][TEXUTRE] Support multiple formats
    std::string path = std::format(pathFormat, textureName, "png");

    Texture temp;
    muint8* data = stbi_load
    (
        path.data,
        (muint32*)&temp.m_Width,
        (muint32*)&temp.m_Height,
        (muint32*)&temp.m_ChannelCount,
        channelCount
    );

    temp.m_ChannelCount = channelCount;
    if (data != nullptr) 
    {
        muint32 generation = texture->m_Generation;
        texture->m_Generation = INVALID_ID;

        muint64 size = temp.m_Width * temp.m_Height * channelCount;

        mbool isTransparent = false;
        for (muint64 i = 0; i < size; i += channelCount)
        {
            muint8 alpha = data[i + 3];
            if (alpha < 255) 
            {
                isTransparent = true;
                break;
            }
        }

        // Acquire internal texture resources and upload to GPU.
        Graphics::Renderer* renderer = smRenderer();
        if (renderer == nullptr)
        {
            hardAssert(false, "Renderer is nullptr!");
        }

        renderer->CreateTexture
        (
            textureName,
            temp.m_Width,
            temp.m_Height,
            temp.m_ChannelCount,
            data,
            isTransparent,
            &temp
        );

        Texture oldTexture = *texture;
        *texture = temp;
        renderer->DestroyTexture(&oldTexture);
        
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
        softAssert(false, "Failed to load texture '%s': %s", path.data, reason);
    }
#endif
	return false;
}

END_NAMESPACE
