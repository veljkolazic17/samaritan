#include <engine/graphics/systems/materialsystem.hpp>

#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/systems/shadersystem.hpp>
#include <engine/graphics/systems/texturesystem.hpp>

#ifdef SM_TOOL
#include <engine/graphics/debug/lightingdebug.hpp>
#endif

#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

smbool MaterialSystem::Init()
{
    m_DefaultMaterial.m_Name = SM_DEFAULT_MATERIAL_NAME;
    m_DefaultMaterial.m_InternalID = SM_INVALID_ID;
    m_DefaultMaterial.m_DiffuseColor = smVec4_one;
    m_DefaultMaterial.m_DiffuseMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_DIFFUSE;
    m_DefaultMaterial.m_DiffuseMap.m_Texture = smTextureSystem().GetDefaultTexture();
    m_DefaultMaterial.m_SpecularMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_SPECULAR;
    m_DefaultMaterial.m_SpecularMap.m_Texture = smTextureSystem().GetWhiteTexture();
    m_DefaultMaterial.m_NormalMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_NORMAL;
    m_DefaultMaterial.m_NormalMap.m_Texture = smTextureSystem().GetWhiteTexture();
    m_DefaultMaterial.m_Shininess = 32.0f;
    HACK(m_DefaultMaterial.m_ShaderName = SM_DEFAULT_SHADER_NAME;)

    const ResourceHandle<Shader>& shader = smShaderSystem().GetShader(m_DefaultMaterial.m_ShaderName);
    if (!shader.IsValid())
    {
        softAssert(false, "Shader %s not found!", m_DefaultMaterial.m_ShaderName.c_str());
        return false;
    }

    HACK(smRenderer().GetRendererBackend()->ObjectShaderAcquireInstanceResources(shader.GetResource(), m_DefaultMaterial.m_InternalID);)

    return true;
}

void MaterialSystem::Shutdown()
{
    for (auto& [name, entry] : m_Materials)
        DestroyMaterial(entry.m_Material.get());
    m_Materials.clear();
}

void MaterialSystem::SingletonInit()
{
    Init();
}

void MaterialSystem::SingletonShutdown()
{
    Shutdown();
}

Material* MaterialSystem::Acquire(const smstring& name)
{
    if (name == SM_DEFAULT_MATERIAL_NAME)
        return &m_DefaultMaterial;

    // Return existing
    auto it = m_Materials.find(name);
    if (it != m_Materials.end())
    {
        ++it->second.m_RefCount;
        return it->second.m_Material.get();
    }

    auto mat = std::make_unique<Material>(name);
    mat->OnLoad();
    if (mat->m_State == ResourceState::Error)
    {
        LogError(LogChannel::Graphics, "Failed to load material '%s'!", name.c_str());
        return nullptr;
    }

    const smstring matName = mat->m_Name;
    Entry& entry = m_Materials[matName];
    entry.m_Material = std::move(mat);
    entry.m_RefCount = 1;
    entry.m_ShouldAutoRelease = false;

    return entry.m_Material.get();
}

Material* MaterialSystem::Register(Material material, smbool autoRelease)
{
    const smstring& name = material.m_Name;

    if (name == SM_DEFAULT_MATERIAL_NAME)
        return &m_DefaultMaterial;

    // If already registered, just bump refcount
    auto it = m_Materials.find(name);
    if (it != m_Materials.end())
    {
        ++it->second.m_RefCount;
        return it->second.m_Material.get();
    }

    // Acquire Vulkan descriptor set
    const ResourceHandle<Shader>& shader = smShaderSystem().GetShader(material.m_ShaderName);
    if (!shader.IsValid())
    {
        softAssert(false, "Shader %s not found!", material.m_ShaderName.c_str());
        return &m_DefaultMaterial;
    }
    HACK(smRenderer().GetRendererBackend()->ObjectShaderAcquireInstanceResources(shader.GetResource(), material.m_InternalID);)

    Entry& entry = m_Materials[name];
    entry.m_Material = std::make_unique<Material>(std::move(material));
    entry.m_RefCount = 1;
    entry.m_ShouldAutoRelease = autoRelease;

    return entry.m_Material.get();
}

void MaterialSystem::Release(const smstring& name)
{
    if (name == SM_DEFAULT_MATERIAL_NAME)
    {
        softAssert(false, "Trying to release default material!");
        return;
    }

    auto it = m_Materials.find(name);
    if (it == m_Materials.end())
    {
        softAssert(false, "Material not loaded!");
        return;
    }

    Entry& entry = it->second;
    --entry.m_RefCount;

    if (entry.m_RefCount == 0 && entry.m_ShouldAutoRelease)
    {
        DestroyMaterial(entry.m_Material.get());
        m_Materials.erase(it);
    }
}

void MaterialSystem::DestroyMaterial(Material* material)
{
    if (material == nullptr)
    {
        softAssert(false, "Trying to destroy invalid material!");
        return;
    }

    material->OnUnload();
}

smbool MaterialSystem::ApplyGlobal(const smstring& shaderName, const smMat4& projection, const smMat4& view)
{
    ShaderSystem& shaderSystem = smShaderSystem();
    shaderSystem.SetUniformByName("projection", &projection);
    shaderSystem.SetUniformByName("view", &view);
#ifdef SM_TOOL
    smVec4 ambientColor = smLightingDebug().GetAmbientColor();
#else
    smVec4 ambientColor = smVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
#endif
    shaderSystem.SetUniformByName("ambient_color", &ambientColor);
    smVec4 dirLightDirection = smVec4{ -0.577f, -0.577f, -0.577f, 0.0f };
    smVec4 dirLightColor = smVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    shaderSystem.SetUniformByName("dir_light_direction", &dirLightDirection);
    shaderSystem.SetUniformByName("dir_light_color", &dirLightColor);
    return shaderSystem.ApplyGlobalUniforms();
}

smbool MaterialSystem::ApplyInstance(Material* material)
{
    ShaderSystem& shaderSystem = smShaderSystem();
    shaderSystem.BindInstanceByIndex(material->m_InternalID);
    shaderSystem.SetUniformByName("diffuse_color", &material->m_DiffuseColor);
    shaderSystem.SetUniformByName("shininess", &material->m_Shininess);
    shaderSystem.SetSamplerByName("diffuse_texture", material->m_DiffuseMap.m_Texture);
    shaderSystem.SetSamplerByName("specular_texture", material->m_SpecularMap.m_Texture);
    shaderSystem.SetSamplerByName("normal_texture", material->m_NormalMap.m_Texture);
    return shaderSystem.ApplyInstanceUniforms();
}

smbool MaterialSystem::ApplyLocal(Material* material, const smMat4& model)
{
    return smShaderSystem().SetUniformByName("model", &model);
}

END_NAMESPACE
