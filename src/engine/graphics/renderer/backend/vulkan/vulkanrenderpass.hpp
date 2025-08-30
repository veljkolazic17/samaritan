#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.hpp>

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

	enum VulkanRenderpassClearFlag : smuint8
	{
		CLEAR_NONE = 1 << 0,
		CLEAR_COLOR_BUFFER = 1 << 1,
		CLEAR_DEPTH_BUFFER = 1 << 2,
		CLEAR_STENCIL_BUFFER = 1 << 3
	};

	class VulkanRenderpass
	{
	public:
		void Create(smVec4 renderArea, smVec4 color, smfloat32 depth, smfloat32 stencil, smuint8 clearFlags, smbool hasPreviousPass, smbool hasNextPass);
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

		smuint8 m_ClearFlags = 0;
		smbool m_HasPreviousPass = false;
		smbool m_HasNextPass = false;

		VkRenderPass m_Handle;
	};
}

END_NAMESPACE