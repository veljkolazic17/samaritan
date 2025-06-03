#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>
#include <engine/graphics/renderer/backend/vulkan/vulkanrenderpass.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkancommandbuffer.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	class VulkanPipeline
	{
	public:
		SM_INLINE VkPipeline GetHandle() { return m_Handle; }
		SM_INLINE VkPipelineLayout& GetPipelineLayout() { return m_PipelineLayout; }

        smbool Create
        (
            VulkanRenderpass* renderpass,
            smuint32 attributeCount,
            VkVertexInputAttributeDescription* attributes,
            smuint32 descriptorSetLayoutCount,
            VkDescriptorSetLayout* descriptorSetLayouts,
            smuint32 stageCount,
            VkPipelineShaderStageCreateInfo* stages,
            VkViewport viewport,
            VkRect2D scissor,
            smbool isWireframe,
            VkDevice logicalDevice,
            VkAllocationCallbacks* allocator
        );
        void Destroy(VkDevice logicalDevice, VkAllocationCallbacks* allocator);
        void Bind(VulkanCommandBuffer* commandBuffer, VkPipelineBindPoint bindPoint);
	private:
		VkPipeline m_Handle;
		VkPipelineLayout m_PipelineLayout;
	};
}

END_NAMESPACE