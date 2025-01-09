#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>
#include <engine/graphics/renderer/backend/vulkan/vulkanpipeline.hpp>

#define OBJECT_SHADER_STAGE_COUNT	2
#define BUILTIN_SHADER_NAME_OBJECT	"Builtin.ObjectShader"

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
		void Create(VulkanRenderpass* renderpass, VkDevice logicalDevice, VkAllocationCallbacks* allocator, mfloat32 framebufferWidth, mfloat32 framebufferHeight);
		void Destroy(VkDevice logicalDevice, VkAllocationCallbacks* allocator);
		void Use(VulkanCommandBuffer* commandBuffer);
	private:
		VulkanShaderStage m_Stages[OBJECT_SHADER_STAGE_COUNT];
		VulkanPipeline m_VulkanPipeline;
	};
}

END_NAMESPACE