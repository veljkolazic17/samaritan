#pragma once
#include <defines.hpp>

#include <engine/resources/graphics/texture.hpp>

#include <objecttemplates/singleton.hpp>
#include <unordered_map>
#include <vector>

#define smTextureSystem()			::samaritan::TextureSystem::GetInstance()

BEGIN_NAMESPACE

class TextureSystem SINGLETON(TextureSystem)
{
public:
    SINGLETON_CONSTRUCTOR(TextureSystem)

    smbool Init();
    void Shutdown();
    void SingletonInit() override;
    void SingletonShutdown() override;

    const ResourceHandle<Texture>& Acquire(const smstring& name, smbool shouldAutoRelease);
    void Release(const smstring & name);

    SM_INLINE ResourceHandle<Texture>& GetDefaultTexture() { return m_DefaultTexture; }

private:
    ResourceHandle<Texture> m_DefaultTexture;
    Texture m_DefaultTextureData;

    std::unordered_map<smstring, ResourceHandle<Texture>> m_TextureLookup;
};

END_NAMESPACE