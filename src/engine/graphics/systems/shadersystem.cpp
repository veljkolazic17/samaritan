#include <engine/graphics/systems/shadersystem.hpp>
#include <engine/resources/resourcesystem.hpp>

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

    shaderPtr->m_State = ShaderState::INITIALIZED;
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
                return true;
            }
            else
            {
                LogError(LogChannel::Graphics, "Shader %s is not valid!", shaderName);
                return false;
            }
        }
        else
        {
            LogError(LogChannel::Graphics, "Shader %s not found!", shaderName);
            return false;
        }
    }
    return true; // Already using this shader
}

const ResourceHandle<Shader>& ShaderSystem::GetShader(const std::string& shaderName)
{

}

smuint16 ShaderSystem::GetUniformIndex(const ResourceHandle<Shader>& shader, const std::string& uniformName)
{

}

smbool ShaderSystem::SetUniformByName(const std::string& uniformName, const void* value)
{

}

smbool ShaderSystem::SetUniformByIndex(smuint16 uniformIndex, const void* value)
{

}

smbool ShaderSystem::SetSamplerByName(const std::string& samplerName, const ResourceHandle<Texture>& texture)
{

}

smbool ShaderSystem::SetSamplerByIndex(smuint16 samplerIndex, const ResourceHandle<Texture>& texture)
{

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
            return smRenderer().ObjectShaderApplyGlobals(shader.GetResource());
        }
        LogError(LogChannel::Graphics, "Shader %s is not valid!", m_CurrentShaderName);
        return false;
    }
    LogError(LogChannel::Graphics, "Shader %s not found!", m_CurrentShaderName);
    return false
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