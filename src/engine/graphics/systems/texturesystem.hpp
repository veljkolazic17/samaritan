#pragma once
#include <defines.hpp>

#include <engine/resources/graphics/texture.hpp>

#include <objecttemplates/singleton.hpp>
#include <unordered_map>
#include <vector>

#define smTextureSystem()			::samaritan::TextureSystem::GetInstance()

BEGIN_NAMESPACE

//TODO : [GRAPHICS] Make reference system for whole engine
struct TextureReference
{
	muint64 m_RefCount = 0;
	muint32 m_Handle = SM_INVALID_ID;
	mbool m_ShouldAutoRelease = false;
};

struct TextureSystemConfing
{
	muint32 m_MaxTextureCount = 0;
};

class TextureSystem SINGLETON(TextureSystem)
{
public:
	SINGLETON_CONSTRUCTOR(TextureSystem)

	mbool Init(const TextureSystemConfing& config);
	void Shutdown();

	Texture* Acquire(mcstring name, mbool shouldAutoRelease);
	void Release(mcstring name);

	SM_INLINE Texture& GetDefaultTexture() { return m_DefaultTexture; }
private:
	mbool LoadTexture(mcstring textureName, Texture * texture);

private:
	Texture m_DefaultTexture;

	TextureSystemConfing m_Config;

	//TODO : [FUCKED][GRAPHICS] Do not save textures like this
	std::vector<Texture> m_Textures;
	std::unordered_map<mcstring, TextureReference> m_TextureLookup;
};

END_NAMESPACE