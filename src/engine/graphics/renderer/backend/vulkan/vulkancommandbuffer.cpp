#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkancommandbuffer.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	void VulkanCommandBuffer::AllocateBuffer(VkDevice device, VkCommandPool commandPool, mbool isPrimary)
	{
		constexpr muint32 numberOfCommandBuffers = 1;

		m_Handle = 0;
		m_State = VulkanCommandBufferState::COMMAND_BUFFER_STATE_NOT_ALLOCATED;

		VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.commandPool = commandPool;
		allocateInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocateInfo.commandBufferCount = numberOfCommandBuffers;
		allocateInfo.pNext = 0;
		VulkanCheckResult(vkAllocateCommandBuffers(device, &allocateInfo, &m_Handle), "Could not create Command Buffer!");
		m_State = VulkanCommandBufferState::COMMAND_BUFFER_STATE_READY;
	}

	void VulkanCommandBuffer::FreeBuffer(VkDevice device, VkCommandPool commandPool)
	{
		constexpr muint32 numberOfCommandBuffers = 1;

		vkFreeCommandBuffers(device, commandPool, numberOfCommandBuffers, &m_Handle);
		m_Handle = 0;
		m_State = VulkanCommandBufferState::COMMAND_BUFFER_STATE_NOT_ALLOCATED;
	}

	void VulkanCommandBuffer::BeginBuffer(mbool isSingleUse, mbool isRenderpassContinue, mbool isSimultaneousUse)
	{
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = 0;
		
		if (isSingleUse)
		{
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		}
		if (isRenderpassContinue)
		{
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		}
		if (isSimultaneousUse)
		{
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		}

		VulkanCheckResult(vkBeginCommandBuffer(m_Handle, &beginInfo), "Could not start recording Command Buffer!");
		m_State = VulkanCommandBufferState::COMMAND_BUFFER_STATE_RECORDING;
	}

	void VulkanCommandBuffer::EndBuffer()
	{
		VulkanCheckResult(vkEndCommandBuffer(m_Handle), "Could not end recording Command Buffer!");
		m_State = VulkanCommandBufferState::COMMAND_BUFFER_STATE_RECORDING_ENDED;
	}

	void VulkanCommandBuffer::BeginSingleUseBuffer(VkDevice device, VkCommandPool commandPool)
	{
		constexpr mbool isPrimary = true;
		constexpr mbool isSingleUse = true;
		constexpr mbool isRenderpassContinue = false;
		constexpr mbool isSimultaneousUse = false;

		AllocateBuffer(device, commandPool, isPrimary);
		BeginBuffer(isSingleUse, isRenderpassContinue, isSimultaneousUse);
	}

	void VulkanCommandBuffer::EndSingleUseBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue)
	{
		constexpr mbool numberOfCommands = 1;

		EndBuffer();
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_Handle;
		VulkanCheckResult(vkQueueSubmit(queue, numberOfCommands, &submitInfo, 0), "Command could not be submitted!");
		VulkanCheckResult(vkQueueWaitIdle(queue), "Error while waiting command to finish!");
		FreeBuffer(device, commandPool);
	}
}

END_NAMESPACE