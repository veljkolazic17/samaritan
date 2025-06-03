#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>

BEGIN_NAMESPACE

namespace Graphics
{
	enum class VulkanCommandBufferState
	{
		COMMAND_BUFFER_STATE_READY,
		COMMAND_BUFFER_STATE_RECORDING,
		COMMAND_BUFFER_STATE_IN_RENDER_PASS,
		COMMAND_BUFFER_STATE_RECORDING_ENDED,
		COMMAND_BUFFER_STATE_SUBMITTED,
		COMMAND_BUFFER_STATE_NOT_ALLOCATED,
		SIZE
	};

	class VulkanCommandBuffer
	{
	public:
		SM_INLINE VkCommandBuffer& GetHandle() { return m_Handle; }
		SM_INLINE void SetState(VulkanCommandBufferState state) { m_State = state; }
		SM_INLINE VulkanCommandBufferState& GetState() { return m_State; }

		void AllocateBuffer(VkDevice device, VkCommandPool commandPool, smbool isPrimary);
		void FreeBuffer(VkDevice device, VkCommandPool commandPool);
		void BeginBuffer(smbool isSingleUse, smbool isRenderpassContinue, smbool isSimultaneousUse);
		void EndBuffer();
		void BeginSingleUseBuffer(VkDevice device, VkCommandPool commandPool);
		void EndSingleUseBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue);
		SM_INLINE void ResetBuffer()
		{
			m_State = VulkanCommandBufferState::COMMAND_BUFFER_STATE_READY;
		}
		SM_INLINE void Submit()
		{
			m_State = VulkanCommandBufferState::COMMAND_BUFFER_STATE_READY;
		}
	private:
		VkCommandBuffer m_Handle;
		VulkanCommandBufferState m_State = VulkanCommandBufferState::COMMAND_BUFFER_STATE_NOT_ALLOCATED;
	};
}

END_NAMESPACE