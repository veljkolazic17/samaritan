#pragma once
#include <defines.hpp>

#include <engine/resources/graphics/material.hpp>
#include <engine/resources/graphics/shader.hpp>

#include <math/matrix.hpp>
#include <objecttemplates/singleton.hpp>
#include <memory>
#include <unordered_map>

BEGIN_NAMESPACE

#define smMaterialSystem()			::samaritan::MaterialSystem::GetInstance()

class MaterialSystem SINGLETON(MaterialSystem)
{
public:
    SINGLETON_CONSTRUCTOR(MaterialSystem)

    smbool Init();
    void Shutdown();
    void SingletonInit() override;
    void SingletonShutdown() override;

    // Load material from JSON file in assets/materials/
    Material* Acquire(const smstring& name);

    // Register a fully constructed material (textures already loaded by caller).
    // MaterialSystem acquires the Vulkan descriptor set and owns cleanup.
    Material* Register(Material material, smbool autoRelease = false);

    void Release(const smstring& name);

    const Material& GetDefaultMaterial() const { return m_DefaultMaterial; }
    Material& GetDefaultMaterial() { return m_DefaultMaterial; }

#ifdef SM_TOOL
    const auto& GetMaterials() const { return m_Materials; }
#endif

private:
    struct Entry
    {
        std::unique_ptr<Material> m_Material;
        smuint32 m_RefCount = 0;
        smbool m_ShouldAutoRelease = false;
    };

    void DestroyMaterial(Material* material);

    Material m_DefaultMaterial;
    std::unordered_map<smstring, Entry> m_Materials;
};

END_NAMESPACE
