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
		SM_INLINE VkCommandBuffer& GetCommandBuffer() { return m_Handle; }
		SM_INLINE VulkanCommandBufferState& SetState(VulkanCommandBufferState state) { m_State = state; }
		SM_INLINE VulkanCommandBufferState& GetState() { return m_State; }
	private:
		VkCommandBuffer m_Handle;
		VulkanCommandBufferState m_State;
	};
}

END_NAMESPACE