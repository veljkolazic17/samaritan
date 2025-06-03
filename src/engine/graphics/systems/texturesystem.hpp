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
	smuint64 m_RefCount = 0;
	smuint32 m_Handle = SM_INVALID_ID;
	smbool m_ShouldAutoRelease = false;
};

struct TextureSystemConfing
{
	smuint32 m_MaxTextureCount = 0;
};

class TextureSystem SINGLETON(TextureSystem)
{
public:
	SINGLETON_CONSTRUCTOR(TextureSystem)

	smbool Init(const TextureSystemConfing& config);
	void Shutdown();

	Texture* Acquire(const smstring& name, smbool shouldAutoRelease);
	void Release(const smstring& name);

	SM_INLINE Texture& GetDefaultTexture() { return m_DefaultTexture; }
private:
	smbool LoadTexture(const smstring& textureName, Texture * texture);

private:
	Texture m_DefaultTexture;

	TextureSystemConfing m_Config;

	//TODO : [FUCKED][GRAPHICS] Do not save textures like this
	std::vector<Texture> m_Textures;
	std::unordered_map<smstring, TextureReference> m_TextureLookup;
};

END_NAMESPACE