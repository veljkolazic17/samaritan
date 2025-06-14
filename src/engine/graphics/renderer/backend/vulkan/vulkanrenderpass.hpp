#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>

#include <math/vector.hpp>

#define DEFAULT_CLEAR_COLOR	{0.2f, 0.2f, 0.2f, 1.0f}

BEGIN_NAMESPACE

namespace Graphics
{
	class VulkanRenderer;
	class VulkanCommandBuffer;

	enum class VulkanRenderpassState
	{
		NOT_ALLOCATED,
		READY,
		RECORDING,
		IN_RENDER_PASS,
		RECORDING_ENDED,
		SUBMITTED,
		SIZE
	};

	class VulkanRenderpass
	{
	public:
		void Create(smVec4 renderArea, smVec4 color, smfloat32 depth, smfloat32 stencil);
		void Destroy();

		void Begin(smVec4 renderArea, VulkanCommandBuffer& commandBuffer, VkFramebuffer frameBuffer);
		void End(VulkanCommandBuffer& commandBuffer);

		SM_INLINE VkRenderPass GetHandle() { return m_Handle; }
	private:
		smVec4 m_RenderArea;
		smVec4 m_Color DEFAULT_CLEAR_COLOR;
		smfloat32 m_Depth;
		smfloat32 m_Stencil;
		VulkanRenderpassState m_State = VulkanRenderpassState::NOT_ALLOCATED;

		VkRenderPass m_Handle;
	};
}

END_NAMESPACE