#include <engine/resources/graphics/shader.hpp>  

#include <nlohmann/json.hpp>
#include <fstream>

BEGIN_NAMESPACE  

void Shader::OnUnload()  
{  
}  

void Shader::OnLoad()  
{  
   constexpr smcstring pathFormat = "../../../assets/shaders/{}.json";  
   std::string path = std::format(pathFormat, m_Name);  

   std::ifstream f(path);  
   if (f.fail())  
   {  
       LogError(LogChannel::Graphics, "Cannot open shader file!");  
       return;  
   }  

   nlohmann::json data = nlohmann::json::parse(f);  

   LoadShaderInfo(&data);  
   LoadShaderAttributes(&data);  
   LoadShaderUniforms(&data);  
}  

void Shader::LoadShaderInfo(void* data)  
{  
   const nlohmann::json& dataJson = *reinterpret_cast<nlohmann::json*>(data);  

   m_Name = dataJson["name"].template get<std::string>();  
   m_RenderpassName = dataJson["renderpass"].template get<std::string>();  
   m_IsUsingInsance = dataJson["isUsingInstance"].template get<smbool>();  
   m_IsUsingLocals = dataJson["isUsingLocal"].template get<smbool>();  
}  

void Shader::LoadShaderAttributes(void* data)  
{  
   const nlohmann::json& dataJson = *reinterpret_cast<nlohmann::json*>(data);  

   const nlohmann::json attributes = dataJson["attributes"];  
   if (attributes.is_array())  
   {  
       for (const auto& attribute : attributes)  
       {  
           ShaderAttribute shaderAttribute;  
           shaderAttribute.m_Name = attribute["name"].template get<std::string>();  
           shaderAttribute.m_DataType = ShaderHelper::ShaderDataTypeFromString(attribute["type"].template get<std::string>());  
           shaderAttribute.m_Size = ShaderHelper::ShaderDataTypeSize(shaderAttribute.m_DataType);  

           m_Attributes.push_back(std::move(shaderAttribute));  
       }  
   }  
   else  
   {  
       softAssert(false, "Shader attributes are not an array!");  
   }  
}  

void Shader::LoadShaderUniforms(void* data)  
{  
   const nlohmann::json& dataJson = *reinterpret_cast<nlohmann::json*>(data);  

   const nlohmann::json uniforms = dataJson["uniforms"];  
   if (uniforms.is_array())  
   {  
       for (const auto& uniform : uniforms)  
       {  
           ShaderUniform shaderUniform;  
           const std::string name = uniform["name"].template get<std::string>();  
           shaderUniform.m_DataType = ShaderHelper::ShaderDataTypeFromString(uniform["type"].template get<std::string>());  
           shaderUniform.m_Size = ShaderHelper::ShaderDataTypeSize(shaderUniform.m_DataType);  
           shaderUniform.m_ScopeType = ShaderHelper::ShaderScopeTypeFromString(uniform["scope"].template get<std::string>());  

           m_UniformLookupTable[name] = std::move(shaderUniform);  
       }  
   }  
   else  
   {  
       softAssert(false, "Shader uniforms are not an array!");  
   }  
}  

END_NAMESPACE