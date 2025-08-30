#include <engine/graphics/systems/shadersystem.hpp>
#include <engine/resources/resourcesystem.hpp>

#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/graphics/systems/texturesystem.hpp>
#include <utils/typeutils.hpp>

BEGIN_NAMESPACE

smbool ShaderSystem::Init()
{

}

void ShaderSystem::Shutdown()
{

}

smbool ShaderSystem::Create(const std::string& shaderName)
{
    ResourceHandle<Shader> shader = smResourceSystem().Load(shaderName);
    m_ShaderLookup[shaderName] = std::move(shader);

    Shader* shaderPtr = shader.GetResource();
    shaderPtr->m_State = ShaderState::NOT_CREATED;

    //Create Shader;

    shaderPtr->m_PushConstantStride = 128;
    shaderPtr->m_PushConstantSize = 0;

    shaderPtr->m_State = ShaderState::UNINITIALIZED;

    if (!smRenderer().CreateShader(shaderPtr))
    {
        softAssert(false, "Failed to create shader: %s", shaderName);
        return false;
    }

    for (ShaderAttribute& attribute : shaderPtr->m_Attributes)
    {
        InitAttribute(*shaderPtr, attribute);
    }

    for (ShaderUniform& uniform : shaderPtr->m_Uniforms)
    {
        if (uniform.m_Type == ShaderDataType::SAMPLER)
        {
            InitSampler(*shaderPtr, uniform);
        }
        else
        {
            InitUniform(*shaderPtr, uniform);
        }
    }

    shaderPtr->m_State = ShaderState::INITIALIZED;
}

void ShaderSystem::InitAttribute(Shader& shader, ShaderAttribute& attribute)
{
    shader.m_AttributeStride += attribute.m_Size;
}

void ShaderSystem::InitUniform(Shader& shader, ShaderUniform& uniform)
{
    if (uniform.m_ScopeType != ShaderScopeType::LOCAL)
    {
        uniform.m_SetIndex = static_cast<smuint32>(uniform.m_ScopeType);
        if (uniform.m_DataType != ShaderDataType::SAMPLER)
        {
            if (uniform.m_ScopeType == ShaderScopeType::GLOBAL)
            {
                uniform.m_Offset = shader.m_GlobalUBOSize;
                shader.m_GlobalUBOSize += uniform.m_Size;
            }
            else
            {
                uniform.m_Offset = shader.m_UBOSize;
                shader.m_UBOSize += uniform.m_Size;
            }
        }
        else
        {
            uniform.m_Size = 0;
        }
    }
    else // ShaderScopeType::LOCAL
    {
        if (!shader.m_IsUsingLocals)
        {
            softAssert(false, "Shader %s is not using local uniforms but has local uniform %s", shader.m_Name.c_str(), uniform.m_Name.c_str());
            return;
        }

        uniform.m_SetIndex = SM_INVALID_ID; // Local uniforms do not have a set index

        //https://registry.khronos.org/vulkan/specs/latest/man/html/VkPushConstantRange.html
        constexpr smuint32 vulkanPushConstantAlligment = 4;
        Range range = TypeUtils::GetAllignedRange(shader.m_PushConstantSize, uniform.m_Size, vulkanPushConstantAlligment);

        uniform.m_Offset = range.m_Offset;
        uniform.m_Size = range.m_Size;

        shader.m_PushConstantRanges[shader.m_PushConstantRangeCount++] = range;
        shader.m_PushConstantStride += range.m_Size;
    }


}

void ShaderSystem::InitSampler(Shader& shader, ShaderUniform& sampler)
{
    if (sampler.m_ScopeType == ShaderScopeType::INSTANCE && !shader.m_IsUsingInsance)
    {
        softAssert(false, "Shader %s is not using instance scope but has instance uniform %s", shader.m_Name.c_str(), sampler.m_Name.c_str());
        return;
    }

    if (sampler.m_ScopeType == ShaderScopeType::LOCAL)
    {
        softAssert(false, "Shader %s has local sampler %s, which is not supported!", shader.m_Name.c_str(), sampler.m_Name.c_str());
        return;
    }

    if (sampler.m_ScopeType == ShaderScopeType::GLOBAL)
    {
        shader.m_GlobalTextures.push_back(smTextureSystem().GetDefaultTexture());
    }
    else // ShaderScopeType::INSTANCE
    {
        ++shader.m_InstanceTextureCount;
    }

    InitUniform(shader, sampler);
}

smbool ShaderSystem::Use(const std::string& shaderName)
{
    if (m_CurrentShaderName != shaderName)
    {
        auto it = m_ShaderLookup.find(shaderName);
        if (it != m_ShaderLookup.end())
        {
            const ResourceHandle<Shader>& shader = it->second;
            if (shader.IsValid())
            {
                if (!smRenderer().UseObjectShader(shader.GetResource()))
                {
                    softAssert(false, "Failed to use shader: %s", shaderName);
                    return false;
                }

                if (!smRenderer().ObjectShaderBindGlobals(shader))
                {
                    softAssert(false, "Failed to bind global uniforms for shader: %s", shaderName);
                    return false;
                }

                m_CurrentShaderName = shaderName;
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

const ResourceHandle<Shader>& ShaderSystem::GetShader(const std::string& shaderName)
{
    auto it = m_ShaderLookup.find(shaderName);
    if (it != m_ShaderLookup.end())
    {
        return it->second;
    }
    LogError(LogChannel::Graphics, "Shader %s not found!", shaderName);
    return ResourceHandle<Shader>::InvalidReference();
}

smbool ShaderSystem::SetUniformByName(const std::string& uniformName, const void* value)
{
    auto it = m_ShaderLookup.find(m_CurrentShaderName);
    if (it != m_ShaderLookup.end())
    {
        const ResourceHandle<Shader>& shader = it->second;
        if (shader.IsValid())
        {
            Shader* shaderPtr = shader.GetResource();
            const ShaderUniform& shaderUniform = shaderPtr->m_UniformLookupTable[uniformName];

            if (shaderPtr->m_ScopeType != shaderUniform.m_ScopeType)
            {
                if (shaderUniform.m_ScopeType == ShaderScopeType::GLOBAL)
                {
                    HACK(smRenderer().GetRendererBackend()->ObjectShaderBindGlobals(shaderPtr);)
                }
                else if (shaderUniform.m_ScopeType == ShaderScopeType::INSTANCE)
                {
                    HACK(smRenderer().GetRendererBackend()->ObjectShaderBindInstances(shaderPtr);)
                }
                shaderPtr->m_ScopeType = shaderUniform.m_ScopeType;
            }

            HACK(return smRenderer().GetRendererBackend()->ObjectShaderSetUniform(shaderPtr, uniform, value);)
        }
    }
    return false;
}

smbool ShaderSystem::SetSamplerByName(const std::string& samplerName, const ResourceHandle<Texture>& texture)
{
    SetUniformByName(samplerName, texture.GetResource());
}

smbool ShaderSystem::ApplyGlobalUniforms()
{
    if (m_CurrentShaderName.empty())
    {
        LogError(LogChannel::Graphics, "No shader is currently in use!");
        return false;
    }

    auto it = m_ShaderLookup.find(m_CurrentShaderName);
    if (it != m_ShaderLookup.end())
    {
        const ResourceHandle<Shader>& shader = it->second;
        if (shader.IsValid())
        {
            HACK(return smRenderer().GetRendererBackend()->ObjectShaderApplyGlobals(shader.GetResource());)
        }
        LogError(LogChannel::Graphics, "Shader %s is not valid!", m_CurrentShaderName);
        return false;
    }
    LogError(LogChannel::Graphics, "Shader %s not found!", m_CurrentShaderName);
    return false;
}

smbool ShaderSystem::ApplyInstanceUniforms()
{
    if (m_CurrentShaderName.empty())
    {
        LogError(LogChannel::Graphics, "No shader is currently in use!");
        return false;
    }

    auto it = m_ShaderLookup.find(m_CurrentShaderName);
    if (it != m_ShaderLookup.end())
    {
        const ResourceHandle<Shader>& shader = it->second;
        if (shader.IsValid())
        {
            return smRenderer().ObjectShaderApplyInstances(shader.GetResource());
        }
        LogError(LogChannel::Graphics, "Shader %s is not valid!", m_CurrentShaderName);
        return false;
    }
    LogError(LogChannel::Graphics, "Shader %s not found!", m_CurrentShaderName);
    return false;
}

smbool ShaderSystem::BindInstanceByIndex(smuint32 instanceId)
{
    auto it = m_ShaderLookup.find(m_CurrentShaderName);
    if (it != m_ShaderLookup.end())
    {
        const ResourceHandle<Shader>& shader = it->second;
        if (shader.IsValid())
        {
            return smRenderer().ObjectShaderBindInstance(shader.GetResource(), instanceId);
        }
        LogError(LogChannel::Graphics, "Shader %s is not valid!", m_CurrentShaderName);
        return false;
    }
    LogError(LogChannel::Graphics, "Shader %s not found!", m_CurrentShaderName);
    return false;
}

END_NAMESPACE