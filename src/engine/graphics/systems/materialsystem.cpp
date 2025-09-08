#include <engine/graphics/systems/materialsystem.hpp>

#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/systems/shadersystem.hpp>
#include <engine/graphics/systems/texturesystem.hpp>

#include <fstream>
#include <nlohmann/json.hpp>
#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

smbool MaterialSystem::Init(const MaterialSystemConfig& config)
{
    if (config.m_MaxMaterialCount == 0)
    {
        softAssert(false, "Material System Config has max material count 0!");
        return false;
    }

    m_Config = config;

    Material temp;
    std::fill_n(std::back_inserter(m_Materials), config.m_MaxMaterialCount, temp);

    //Create default material
    std::strncpy(reinterpret_cast<char*>(m_DefaultMaterial.m_Name), SM_DEFAULT_MATERIAL_NAME, SM_MATERIAL_NAME_MAX_LENGTH);
    m_DefaultMaterial.m_ID = SM_INVALID_ID;
    m_DefaultMaterial.m_Generation = SM_INVALID_ID;
    m_DefaultMaterial.m_DiffuseColor = smVec4_one;
    m_DefaultMaterial.m_DiffuseMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_DIFFUSE;
    m_DefaultMaterial.m_DiffuseMap.m_Texture = smTextureSystem().GetDefaultTexture();
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
    for (Material& material : m_Materials)
    {
        DestroyMaterial(&material);
    }
}

Material* MaterialSystem::Acquire(const smstring& name)
{
    MaterialConfig materialConfig;
    if (LoadConfigurationFile(name.data(), materialConfig))
    {
        return AcquireFromConfig(materialConfig);
    }
    return nullptr;
}

Material* MaterialSystem::AcquireFromConfig(const MaterialConfig& materialConfig)
{
    if (!std::strcmp(reinterpret_cast<const char*>(materialConfig.m_Name), SM_DEFAULT_MATERIAL_NAME))
    {
        return &m_DefaultMaterial;
    }

    MaterialReference& reference = m_MaterialLookup[std::string(reinterpret_cast<smcstring>(materialConfig.m_Name))];

    if (reference.m_RefCount == 0)
    {
        reference.m_ShouldAutoRelease = materialConfig.m_ShouldAutoRelease;
    }

    ++reference.m_RefCount;
    if (reference.m_Handle == SM_INVALID_ID)
    {
        //Find free space in array
        //TODO : [TEXTURE] change this not to be vector
        Material* newMaterial = nullptr;
        for (smuint64 counter = 0; counter < m_Materials.size(); ++counter)
        {
            Material& material = m_Materials[counter];
            if (material.m_ID == SM_INVALID_ID)
            {
                newMaterial = &material;
                reference.m_Handle = counter;
                break;
            }
        }

        if (newMaterial == nullptr)
        {
            softAssert(false, "Can't load more textures!");
            return &m_DefaultMaterial;
        }

        if (!LoadMaterial(materialConfig, newMaterial))
        {
            softAssert(false, "Failed to load texture %s", name);
            return &m_DefaultMaterial;
        }

        newMaterial->m_ID = reference.m_Handle;
        return newMaterial;
    }
    return &m_Materials[reference.m_Handle];
}

void MaterialSystem::Release(const smstring& name)
{
    if (!std::strcmp(name.data(), SM_DEFAULT_MATERIAL_NAME))
    {
        softAssert(false, "Trying to release default material!");
    }

    if (m_MaterialLookup.contains(name))
    {
        MaterialReference& reference = m_MaterialLookup[name];
        --reference.m_RefCount;

        if (reference.m_RefCount == 0 && reference.m_ShouldAutoRelease)
        {
            Material* material = &m_Materials[reference.m_Handle];
            DestroyMaterial(material);
            reference.m_Handle = SM_INVALID_ID;
            reference.m_ShouldAutoRelease = false;
        }
        //TODO : [MATERIAL] Check when reference should be removed from entries
    }
    else
    {
        softAssert(false, "Material not loaded!");
    }
}

smbool MaterialSystem::LoadConfigurationFile(smcstring name, MaterialConfig& config)
{
    constexpr smcstring pathFormat = "../../../assets/materials/{}.json";
    std::string path = std::format(pathFormat, name);

    std::ifstream f(path);
    if (f.fail())
    {
        LogError(LogChannel::Graphics, "Cannot opet material file!");
        return false;
    }

    nlohmann::json data = nlohmann::json::parse(f);

    if (data.size() != 4)
    {
        LogError(LogChannel::Graphics, "Material file has wrong format!");
        return false;
    }

    const std::string& jname = data["name"].template get<std::string>();
    const std::string& jdiffuseMapName = data["diffusemapname"].template get<std::string>();
    const std::string& shaderName = data["shader"].template get<std::string>();

    std::strncpy(reinterpret_cast<char*>(config.m_Name), jname.data(), SM_MATERIAL_NAME_MAX_LENGTH);
    std::strncpy(reinterpret_cast<char*>(config.m_DiffuseMapName), jdiffuseMapName.data(), SM_TEXTURE_NAME_MAX_LENGTH);
    config.m_ShaderName = shaderName;
    config.m_DiffuseColor = smVec4::StringToVec4(data["diffusecolor"]);

    return true;
}

void MaterialSystem::DestroyMaterial(Material* material)
{
    if (material == nullptr)
    {
        softAssert(false, "Trying to destroy invalid material!");
        return;
    }

    if (Texture* texture = material->m_DiffuseMap.m_Texture.GetResource())
    {
        smTextureSystem().Release(texture->m_Name);
    }

    const ResourceHandle<Shader>& shader = smShaderSystem().GetShader(material->m_ShaderName);
    HACK(smRenderer().GetRendererBackend()->ObjectShaderReleaseInstanceResources(shader.GetResource(), material->m_InternalID);)

    material->m_ID = SM_INVALID_ID;
    material->m_Generation = SM_INVALID_ID;
    material->m_InternalID = SM_INVALID_ID;
}

smbool MaterialSystem::LoadMaterial(const MaterialConfig& config, Material* material)
{
    smZero(material, sizeof(Material));
    std::strncpy(reinterpret_cast<char*>(material->m_Name), reinterpret_cast<const char*>(config.m_Name), SM_MATERIAL_NAME_MAX_LENGTH);

    material->m_ShaderName = config.m_ShaderName;

    material->m_DiffuseColor = config.m_DiffuseColor;
    if (std::strlen(reinterpret_cast<const char*>(config.m_DiffuseMapName)) > 0)
    {
        material->m_DiffuseMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_DIFFUSE;
        constexpr smbool shouldAutoRelease = true;
        material->m_DiffuseMap.m_Texture = smTextureSystem().Acquire(reinterpret_cast<smcstring>(config.m_DiffuseMapName), shouldAutoRelease);
        if (!material->m_DiffuseMap.m_Texture.IsValid())
        {
            LogError(LogChannel::Graphics, "Failed to acquire texture");
            material->m_DiffuseMap.m_Texture = smTextureSystem().GetDefaultTexture();
        }
    }
    else
    {
        material->m_DiffuseMap.m_Type = TextureUsageType::TEXTURE_USAGE_UNKNOWN;
        material->m_DiffuseMap.m_Texture = ResourceHandle<Texture>::InvalidReference();
    }

    const ResourceHandle<Shader>& shader = smShaderSystem().GetShader(config.m_ShaderName);
    if (!shader.IsValid())
    {
        softAssert(false, "Shader %s not found!", config.m_ShaderName.c_str());
        return false;
    }

    HACK(smRenderer().GetRendererBackend()->ObjectShaderAcquireInstanceResources(shader.GetResource(), material->m_InternalID);)

    if (material->m_ID == SM_INVALID_ID)
    {
        material->m_Generation = 0;
    }
    else
    {
        ++material->m_Generation;
    }

    return true;
}

smbool MaterialSystem::ApplyGlobal(const smstring& shaderName, const smMat4& projection, const smMat4& view)
{
    ShaderSystem& shaderSystem = smShaderSystem();
    shaderSystem.SetUniformByName("projection", &projection);
    shaderSystem.SetUniformByName("view", &view);
    return shaderSystem.ApplyGlobalUniforms();
}

smbool MaterialSystem::ApplyInstance(Material* material)
{
    ShaderSystem& shaderSystem = smShaderSystem();
    shaderSystem.BindInstanceByIndex(material->m_InternalID);
    shaderSystem.SetUniformByName("diffuse_colour", &material->m_DiffuseColor);
    shaderSystem.SetSamplerByName("diffuse_texture", material->m_DiffuseMap.m_Texture);
    return shaderSystem.ApplyInstanceUniforms();
}

smbool MaterialSystem::ApplyLocal(Material* material, const smMat4& model)
{
    return smShaderSystem().SetUniformByName("model", &model);
}

END_NAMESPACE