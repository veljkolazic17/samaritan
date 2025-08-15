#pragma once
#include <defines.hpp>

#include <engine/resources/resource.hpp>

#include <engine/resources/graphics/texture.hpp>

#include <vector>
#include <unordered_map>

BEGIN_NAMESPACE

enum class ShaderStage : smuint8
{
    VERTEX_STAGE    = 1 << 0,
    GEOMETRY_STAGE  = 1 << 1,
    FRAGMENT_STAGE  = 1 << 2,
    COMPUTE_STAGE   = 1 << 3
};

// Used for attributes and uniforms
enum class ShaderDataType : smuint8
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

enum class ShaderState 
{
    NOT_CREATED,
    UNINITIALIZED,
    INITIALIZED,
};

enum class ShaderScopeType : smuint8
{
    GLOBAL,
    INSTANCE,
    LOCAL
};

struct ShaderUniform
{
    smuint64 m_Offset;
    /**
     * @brief The location to be used as a lookup. Typically the same as the index except for samplers,
     * which is used to lookup texture index within the internal array at the given scope (global/instance).
     */
    smuint16 m_Location;
    /** @brief Index into the internal uniform array. */
    smuint16 m_Index;
    /** @brief The size of the uniform, or 0 for samplers. */
    smuint16 m_Size;
    /** @brief The index of the descriptor set the uniform belongs to (0=global, 1=instance, INVALID_ID=local). */
    smuint8 m_SetIndex;
    /** @brief The scope of the uniform. */
    ShaderScopeType m_ScopeType;
    /** @brief The type of uniform. */
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

    smbool m_IsUsingInsance = false;
    smbool m_IsUsingLocals = false;

    /**
     * @brief The amount of bytes that are required for UBO alignment.
     *
     * This is used along with the UBO size to determine the ultimate
     * stride, which is how much the UBOs are spaced out in the buffer.
     * For example, a required alignment of 256 means that the stride
     * must be a multiple of 256 (true for some nVidia cards).
     */
    smuint64 m_UBOAlignment;

    /** @brief The actual size of the global uniform buffer object. */
    smuint64 m_GlobalUBOSize;
    /** @brief The stride of the global uniform buffer object. */
    smuint64 m_GlobalUBOStride;
    /**
     * @brief The offset in bytes for the global UBO from the beginning
     * of the uniform buffer.
     */
    smuint64 m_GlobalUBOOfset;

    /** @brief The actual size of the instance uniform buffer object. */
    smuint64 m_UBOSize;

    /** @brief The stride of the instance uniform buffer object. */
    smuint64 m_UBOStride;

    /** @brief The total size of all push constant ranges combined. */
    smuint64 m_PushConstantSize;
    /** @brief The push constant stride, aligned to 4 bytes as required by Vulkan. */
    smuint64 m_PushConstantStride;

    /** @brief An array of global texture pointers. Darray */
    std::vector<ResourceHandle<Texture>> m_GlobalTextures;

    /** @brief The number of instance textures. */
    smuint8 m_InstanceTextureCount = 0;

    ShaderScopeType m_ScopeType;

    /** @brief The identifier of the currently bound instance. */
    smuint32 m_BoundInstancId;
    /** @brief The currently bound instance's ubo offset. */
    smuint32 m_BoundUBOOffset;

    /** @brief A hashtable to store uniform index/locations by name. */
    std::unordered_map<std::string, smuint16> m_UniformLookupTable;

    /** @brief An array of uniforms in this shader. Darray. */
    std::vector<ShaderUniform> m_Uniforms;

    /** @brief An array of attributes. Darray. */
    std::vector<ShaderAttribute> m_Attributes;

    /** @brief The internal state of the shader. */
    ShaderStage m_Stage;

    /** @brief The number of push constant ranges. */
    smuint8 m_PushConstantRangeCount;
    /** @brief An array of push constant ranges. */
    Range PushConstantRanges[32];
    /** @brief The size of all attributes combined, a.k.a. the size of a vertex. */
    smuint16 m_AttributeStride;

    /** @brief An opaque pointer to hold renderer API specific data. Renderer is responsible for creation and destruction of this.  */
    void* m_InternalData;


};

END_NAMESPACE