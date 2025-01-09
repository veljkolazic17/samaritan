#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>

#include <vector>
#include <objecttemplates/shutdownable.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	class VulkanRenderpass;

	class VulkanFramebuffer : public Shutdownable
	{
	public:
		void Init() override;
		void Shutdown() override;
		
		SM_INLINE VkFramebuffer GetHandle() { return m_Handle; }

		void Init(const VulkanCreateArguments& arguments, VulkanRenderpass* renderpass, muint32 width, muint32 height, muint32 attachmentCount, const VkImageView attachments[]);
		void Shutdown(const VulkanCreateArguments& arguments);
	private:
		std::vector<VkImageView> m_Attachments;
		const VulkanRenderpass* m_Renderpass;
		VkFramebuffer m_Handle;
		muint32 m_AttachmentCount;
	};
}

END_NAMESPACE