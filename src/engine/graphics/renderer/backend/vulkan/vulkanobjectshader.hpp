#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>
#include <engine/graphics/renderer/backend/vulkan/vulkanpipeline.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbuffer.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanobjectshaderobjectstate.hpp>
#include <engine/graphics/renderer/renderertype.inl>

#define OBJECT_SHADER_STAGE_COUNT						2
#define BUILTIN_SHADER_NAME_OBJECT						"Builtin.ObjectShader"
#define SM_VULKAN_OBJECT_MAX_OBJECT_COUNT				1024

BEGIN_NAMESPACE

namespace Graphics
{
	class VulkanRenderpass;
	class VulkanCommandBuffer;
}

namespace Graphics
{

	struct VulkanShaderStage
	{
		VkShaderModuleCreateInfo m_CrateInfo;
		VkShaderModule m_Handle;
		VkPipelineShaderStageCreateInfo m_ShaderStageCreateInfo;
	};

	//TODO : Make this a struct
	class VulkanObjectShader
	{
	public:
		SM_INLINE void SetGlobalUniformObject(const GlobalUniformObject& globalUbo) { m_GlobalUbo = globalUbo; }
		SM_INLINE GlobalUniformObject& GetGlobalUniformObject() { return m_GlobalUbo; }
		SM_INLINE VulkanPipeline& GetVulkanPipeline() { return m_VulkanPipeline; }
		
		void Create(VulkanRenderpass* renderpass, VkDevice logicalDevice, VkAllocationCallbacks* allocator, mfloat32 framebufferWidth, mfloat32 framebufferHeight);
		void Destroy(VkDevice logicalDevice, VkAllocationCallbacks* allocator);
		void Use(VulkanCommandBuffer* commandBuffer);
		void UpdateGlobalState();
		void UpdateModel(GeometryData data);

		muint32 AcquireObjectShaderResources();
		void ReleaseObjectShaderResources(muint32 objectID);

	private:
		//TODO : [FUCKED][GRAPHICS][SHADER] make pool of indexer or some shit.This will have unexpected behaviour when overflow
		muint32 m_ObjectUniformBufferIndex;
		VulkanShaderStage m_Stages[OBJECT_SHADER_STAGE_COUNT];
		VkDescriptorPool m_GlobalDescriptorPool;
		VkDescriptorSetLayout m_GlobalDescriptorSetLayout;
		VkDescriptorPool m_DescriptorPool;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		// One descriptor set per frame - max 3 for triple-buffering.
		VkDescriptorSet m_GlobalDescriptorSets[3];

		// Global uniform object.
		GlobalUniformObject m_GlobalUbo;
		// Global uniform buffer.
		VulkanBuffer m_GlobalUniformBuffer;
		//Object uniform buffer
		VulkanBuffer m_ObjectUniformBuffer;
		VulkanPipeline m_VulkanPipeline;
		//TODO : [GRAPHICS][SHADER] make this dynamic
		VulkanObjectShaderObjectState m_ObjectStates[SM_VULKAN_OBJECT_MAX_OBJECT_COUNT];
	};
}

END_NAMESPACE