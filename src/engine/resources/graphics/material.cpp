#include <engine/resources/graphics/material.hpp>

#include <engine/graphics/systems/shadersystem.hpp>
#include <engine/graphics/systems/texturesystem.hpp>
#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>

#include <fstream>
#include <format>
#include <nlohmann/json.hpp>
#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

void Material::OnLoad()
{
    const std::string path = std::format("assets/materials/{}.json", m_Name);
    std::ifstream f(path);
    if (f.fail())
    {
        LogError(LogChannel::Graphics, "Cannot open material file '%s'!", path.c_str());
        m_State = ResourceState::Error;
        return;
    }

    nlohmann::json data = nlohmann::json::parse(f);

    m_Name       = data["name"].template get<std::string>();
    m_ShaderName = data["shader"].template get<std::string>();
    m_DiffuseColor = smVec4::StringToVec4(data["diffusecolor"]);
    m_Shininess  = data.contains("shininess") ? data["shininess"].template get<smfloat32>() : 32.0f;

    // Diffuse texture
    m_DiffuseMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_DIFFUSE;
    if (data.contains("diffusemapname") && !data["diffusemapname"].template get<std::string>().empty())
    {
        const std::string texName = data["diffusemapname"].template get<std::string>();
        m_DiffuseMap.m_Texture = smTextureSystem().Acquire(texName, true);
        if (!m_DiffuseMap.m_Texture.IsValid())
            m_DiffuseMap.m_Texture = smTextureSystem().GetWhiteTexture();
    }
    else
    {
        m_DiffuseMap.m_Texture = smTextureSystem().GetWhiteTexture();
    }

    // Specular texture
    m_SpecularMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_SPECULAR;
    if (data.contains("specularmapname") && !data["specularmapname"].template get<std::string>().empty())
    {
        const std::string texName = data["specularmapname"].template get<std::string>();
        m_SpecularMap.m_Texture = smTextureSystem().Acquire(texName, true);
        if (!m_SpecularMap.m_Texture.IsValid())
            m_SpecularMap.m_Texture = smTextureSystem().GetWhiteTexture();
    }
    else
    {
        m_SpecularMap.m_Texture = smTextureSystem().GetWhiteTexture();
    }

    // Normal texture
    m_NormalMap.m_Type = TextureUsageType::TEXTURE_USAGE_MAP_NORMAL;
    if (data.contains("normalmapname") && !data["normalmapname"].template get<std::string>().empty())
    {
        const std::string texName = data["normalmapname"].template get<std::string>();
        m_NormalMap.m_Texture = smTextureSystem().Acquire(texName, true);
        if (!m_NormalMap.m_Texture.IsValid())
            m_NormalMap.m_Texture = smTextureSystem().GetWhiteTexture();
    }
    else
    {
        m_NormalMap.m_Texture = smTextureSystem().GetWhiteTexture();
    }

    // Acquire Vulkan descriptor set
    const ResourceHandle<Shader>& shader = smShaderSystem().GetShader(m_ShaderName);
    if (!shader.IsValid())
    {
        softAssert(false, "Shader %s not found!", m_ShaderName.c_str());
        m_State = ResourceState::Error;
        return;
    }
    smRenderer().GetRendererBackend()->ShaderAcquireInstanceResources(shader.GetResource(), m_InternalID);

    m_Type  = ResourceType::Material;
    m_State = ResourceState::Loaded;
}

void Material::Apply()
{
    ShaderSystem& shaderSystem = smShaderSystem();
    shaderSystem.BindInstanceByIndex(m_InternalID);
    shaderSystem.SetUniformByName("diffuse_color", &m_DiffuseColor);
    shaderSystem.SetUniformByName("shininess", &m_Shininess);
    shaderSystem.SetSamplerByName("diffuse_texture", m_DiffuseMap.m_Texture);
    shaderSystem.SetSamplerByName("specular_texture", m_SpecularMap.m_Texture);
    shaderSystem.SetSamplerByName("normal_texture", m_NormalMap.m_Texture);
    shaderSystem.ApplyInstanceUniforms();
}

void Material::OnUnload()
{
    if (Texture* texture = m_DiffuseMap.m_Texture.GetResource())
        smTextureSystem().Release(texture->m_Name);

    if (Texture* texture = m_SpecularMap.m_Texture.GetResource())
        smTextureSystem().Release(texture->m_Name);

    if (Texture* texture = m_NormalMap.m_Texture.GetResource())
        smTextureSystem().Release(texture->m_Name);

    const ResourceHandle<Shader>& shader = smShaderSystem().GetShader(m_ShaderName);
    smRenderer().GetRendererBackend()->ShaderReleaseInstanceResources(shader.GetResource(), m_InternalID);

    m_InternalID = SM_INVALID_ID;
}

END_NAMESPACE
