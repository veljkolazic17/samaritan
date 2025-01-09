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

        mbool Create
        (
            VulkanRenderpass* renderpass,
            muint32 attributeCount,
            VkVertexInputAttributeDescription* attributes,
            muint32 descriptorSetLayoutCount,
            VkDescriptorSetLayout* descriptorSetLayouts,
            muint32 stageCount,
            VkPipelineShaderStageCreateInfo* stages,
            VkViewport viewport,
            VkRect2D scissor,
            mbool isWireframe,
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