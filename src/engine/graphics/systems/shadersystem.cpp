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

    //Continue tommorow [CHECKPOINT]

}

smbool ShaderSystem::Use(const std::string& shaderName)
{

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

}

smbool ShaderSystem::ApplyInstanceUniforms()
{

}

smbool ShaderSystem::ApplyInstanceUniforms()
{

}

smbool ShaderSystem::BindInstanceByIndex(smuint32 instanceId)
{

}

END_NAMESPACE