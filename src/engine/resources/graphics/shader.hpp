#pragma once
#include <defines.hpp>

#include <engine/resources/resource.hpp>
#include <engine/resources/graphics/texture.hpp>

#include <math/vector.hpp>
#include <math/matrix.hpp>

#include <vector>
#include <unordered_map>

#define SM_DEFAULT_SHADER_NAME          "ObjectShader"
#define SM_DEFAULT_SHADER_RESOURCE      "ObjectShaderConfig"


BEGIN_NAMESPACE

enum ShaderStageType : smuint8
{
    VERTEX      = 1 << 0,
    GEOMETRY    = 1 << 1,
    FRAGMENT    = 1 << 2,
    COMPUTE     = 1 << 3
};

struct ShaderStage
{
    ShaderStageType m_StageType;
    std::string m_FilePath;
};

// Used for attributes and uniforms
enum ShaderDataType
{
    FLOAT32,
    FLOAT32_2,
    FLOAT32_3,
    FLOAT32_4,
    MATRIX_4,
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    SAMPLER
};

enum ShaderState
{
    NOT_CREATED,
    UNINITIALIZED,
    INITIALIZED,
};

enum ShaderScopeType
{
    GLOBAL,
    INSTANCE,
    LOCAL
};

namespace ShaderHelper
{
    SM_INLINE ShaderDataType ShaderDataTypeFromString(const std::string& type)
    {
        static const std::unordered_map<std::string, ShaderDataType> typeMap =
        {
            {"float", ShaderDataType::FLOAT32},
            {"vec2", ShaderDataType::FLOAT32_2},
            {"vec3", ShaderDataType::FLOAT32_3},
            {"vec4", ShaderDataType::FLOAT32_4},
            {"mat4", ShaderDataType::MATRIX_4},
            {"int8", ShaderDataType::INT8},
            {"uint8", ShaderDataType::UINT8},
            {"int16", ShaderDataType::INT16},
            {"uint16", ShaderDataType::UINT16},
            {"int32", ShaderDataType::INT32},
            {"uint32", ShaderDataType::UINT32},
            {"sampler", ShaderDataType::SAMPLER}
        };

        auto it = typeMap.find(type);
        if (it != typeMap.end())
            return it->second;

        softAssert(false, "Unknown shader data type: %s", type.c_str());
        return ShaderDataType::FLOAT32; // Default fallback
    }

    SM_INLINE smuint32 ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::FLOAT32:      return sizeof(smfloat32);
        case ShaderDataType::FLOAT32_2:    return sizeof(smVec2);
        case ShaderDataType::FLOAT32_3:    return sizeof(smVec3);
        case ShaderDataType::FLOAT32_4:    return sizeof(smVec4);
        case ShaderDataType::MATRIX_4:     return sizeof(smMat4);
        case ShaderDataType::INT8:         return sizeof(smint8);
        case ShaderDataType::UINT8:        return sizeof(smuint8);
        case ShaderDataType::INT16:        return sizeof(smint16);
        case ShaderDataType::UINT16:       return sizeof(smuint16);
        case ShaderDataType::INT32:        return sizeof(smint32);
        case ShaderDataType::UINT32:       return sizeof(smuint32);
        case ShaderDataType::SAMPLER:      return 0; // Samplers have no size
        }
        softAssert(false, "Unknown shader data type size!");
        return 0;
    }

    SM_INLINE ShaderScopeType ShaderScopeTypeFromString(const std::string& scopeType)
    {
        static const std::unordered_map<std::string, ShaderScopeType> scopeMap =
        {
            {"global", ShaderScopeType::GLOBAL},
            {"instance", ShaderScopeType::INSTANCE},
            {"local", ShaderScopeType::LOCAL}
        };

        auto it = scopeMap.find(scopeType);
        if (it != scopeMap.end())
            return it->second;

        softAssert(false, "Unknown shader scope type: %s", scopeType.c_str());
        return ShaderScopeType::GLOBAL; // Default fallback
    }
}

struct ShaderUniform
{
    std::string m_Name;
    smuint64 m_Offset;
    smuint16 m_Location;
    smuint16 m_Index;
    smuint16 m_Size;
    smuint8 m_SetIndex;
    ShaderScopeType m_ScopeType;
    ShaderDataType m_DataType;
};

struct ShaderAttribute
{
    std::string m_Name;
    ShaderDataType m_DataType;
    smuint32 m_Size;
};

struct Shader : Resource
{
    void OnUnload() override;
    void OnLoad() override;

    Shader() = default;

    Shader(const std::string& name)
        : Resource(name)
    {
    }

    smbool m_IsUsingInsance = false;
    smbool m_IsUsingLocals = false;

    //TODO : [SHADER] Make this be ID somehow
    std::string m_RenderpassName;
    smuint64 m_UBOAlignment;
    smuint64 m_GlobalUBOSize;
    smuint64 m_GlobalUBOStride;
    smuint64 m_GlobalUBOOfset;
    smuint64 m_UBOSize;
    smuint64 m_UBOStride;
    smuint64 m_PushConstantSize;
    smuint64 m_PushConstantStride;

    std::vector<ResourceHandle<Texture>> m_GlobalTextures;

    smuint8 m_InstanceTextureCount = 0;

    ShaderScopeType m_ScopeType;

    smuint32 m_BoundInstancId;
    smuint32 m_BoundUBOOffset;

    std::unordered_map<std::string, ShaderUniform> m_UniformLookupTable;

    std::vector<ShaderAttribute> m_Attributes;

    std::vector<ShaderStage> m_Stages;

    smuint8 m_PushConstantRangeCount;
    Range m_PushConstantRanges[32];
    smuint16 m_AttributeStride;

    smstring m_ShaderName;

    void* m_InternalData;

private:
    void LoadShaderInfo(void* data);
    void LoadShaderAttributes(void* data);
    void LoadShaderUniforms(void* data);
};

END_NAMESPACE