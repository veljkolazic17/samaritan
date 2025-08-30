#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkantypes.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbuffer.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanpipeline.hpp>

#define SM_VULKAN_MAX_SHADER_STAGES_COUNT               8
#define SM_VULKAN_MAX_BINDINGS_COUNT                    32
#define SM_VULKAN_MAX_SADER_ATTRIBUTES                  16

#define SM_VULKAN_BINDING_UBO                           0
#define SM_VULKAN_BINDING_SAMPLER                       1

#define SM_VULKAN_DESCRIPTOR_SET_GLOBAL                 0
#define SM_VULKAN_DESCRIPTOR_SET_INSTANCE               1
#define SM_VULKAN_SHADER_MAX_INSTANCE_COUNT             4096

#define SM_VULKAN_SHADER_MAX_GLOBAL_TEXTURES_COUNT      32
#define SM_VULKAN_SHADER_MAX_INSTANCE_TEXTURES_COUNT    32

#define SM_VULKAN_DESCRIPTOR_SET_TYPE_MAX               2 // 0 - global, 1 - instance

BEGIN_NAMESPACE

class Shader;
class VulkanRenderpass;

struct VulkanDescriptorSetData
{
    smuint32 m_Bindings = 0;
    VkDescriptorSetLayoutBinding m_BindingLayouts[SM_VULKAN_MAX_BINDINGS_COUNT];
};

struct VulkanDescriptorState
{
    smuint32 m_Generation[3] = { SM_INVALID_ID, SM_INVALID_ID, SM_INVALID_ID };
    smuint32 m_Id[3] = { SM_INVALID_ID, SM_INVALID_ID, SM_INVALID_ID };
};

struct VulkanDescriptorSetState
{
    VkDescriptorSet m_DescriptorSets[3] = { 0, 0, 0 };
    VulkanDescriptorState m_DescriptorStates[SM_VULKAN_MAX_BINDINGS_COUNT];
};

struct VulkanShaderInstanceState
{
    smuint32 m_Id = SM_INVALID_ID;
    smuint32 m_Generation = SM_INVALID_ID;
    smuint32 m_Offset = 0;
    smuint32 m_Size = 0;

    VulkanDescriptorSetState m_DescriptorSetState;
    std::vector<ResourceHandle<Texture>> m_Textures;
};

struct VulkanShaderStage
{
    VkShaderModuleCreateInfo m_CrateInfo;
    VkShaderModule m_Handle;
    VkPipelineShaderStageCreateInfo m_ShaderStageCreateInfo;
    VkShaderStageFlags m_StageFlag;
};

struct VulkanShader
{
    Shader* m_Shader = nullptr;
    void* m_UniformBufferData = nullptr;
    smuint32 m_Id = SM_INVALID_ID;
    VulkanRenderpass* m_Renderpass = nullptr;

    VkDescriptorPool m_DescriptorPool = 0;
    VkDescriptorSetLayout m_DescriptorSetLayouts[2] = { 0, 0 };
    VkDescriptorSet m_GlobalDescriptorSets[3] = { 0, 0, 0 };

    Graphics::VulkanBuffer m_VulkanUniformBuffer;
    Graphics::VulkanPipeline m_VulkanPipeline;

    smuint32 m_InstanceCount = 0;

    //TODO : [GRAPHICS][SHADER] Make this dynamic
    VkDescriptorPoolSize m_PoolSizes[2];

    const smuint32 m_MaxDescriptorSetCount HACK(= 1024);
    smuint8 m_DescriptorSetCount = 0;

    smuint8 m_StageCount = 0;
    VulkanShaderStage m_Stages[SM_VULKAN_MAX_SHADER_STAGES_COUNT];

    VulkanDescriptorSetData m_DescriptorSetData[SM_VULKAN_DESCRIPTOR_SET_TYPE_MAX];

    VulkanShaderInstanceState m_InstanceStates[SM_VULKAN_SHADER_MAX_INSTANCE_COUNT];

    VkVertexInputAttributeDescription m_Attributes[SM_VULKAN_MAX_SADER_ATTRIBUTES];
};

END_NAMESPACE