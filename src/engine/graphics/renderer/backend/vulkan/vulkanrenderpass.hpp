#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>

#include <math/vector.hpp>

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
		void Create(smVec4 renderArea, smVec4 color, mfloat32 depth, mfloat32 stencil);
		void Destroy();

		void Begin(VulkanCommandBuffer& commandBuffer, VkFramebuffer frameBuffer);
		void End(VulkanCommandBuffer& commandBuffer);

		SM_INLINE VkRenderPass GetHandle() { return m_Handle; }
		SM_INLINE void SetVulkanRenderer(VulkanRenderer* renderer) { m_Renderer = renderer; }
	private:
		smVec4 m_RenderArea;
		smVec4 m_Color;
		mfloat32 m_Depth;
		mfloat32 m_Stencil;
		VulkanRenderpassState m_State = VulkanRenderpassState::NOT_ALLOCATED;

		VkRenderPass m_Handle;

		VulkanRenderer* m_Renderer;
	};
}

END_NAMESPACE