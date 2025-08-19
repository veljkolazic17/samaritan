#pragma once
#include <defines.hpp>

#include <engine/resources/resource.hpp>
#include <engine/resources/graphics/shader.hpp>

#include <objecttemplates/singleton.hpp>


#define smShaderSystem()            ::samaritan::ShaderSystem::GetInstance()

BEGIN_NAMESPACE

class ShaderSystem SINGLETON(ShaderSystem)
{
public:
    SINGLETON_CONSTRUCTOR(ShaderSystem)

    smbool Init();
    void Shutdown();
    
    
    smbool Use(const std::string& shaderName);
    smbool Create(const std::string& shaderName);
    const ResourceHandle<Shader>& GetShader(const std::string& shaderName);

    smuint16 GetUniformIndex(const ResourceHandle<Shader>& shader, const std::string& uniformName);
    
    smbool SetUniformByName(const std::string& uniformName, const void* value);
    smbool SetUniformByIndex(smuint16 uniformIndex, const void* value);
    smbool SetSamplerByName(const std::string& samplerName, const ResourceHandle<Texture>& texture);
    smbool SetSamplerByIndex(smuint16 samplerIndex, const ResourceHandle<Texture>& texture);

    smbool ApplyGlobalUniforms();
    smbool ApplyInstanceUniforms();
    smbool ApplyInstanceUniforms();

    smbool BindInstanceByIndex(smuint32 instanceId);

private:
    std::unordered_map<smstring, ResourceHandle<Shader>> m_ShaderLookup;
    smstring m_CurrentShaderName; 
};

END_NAMESPACE