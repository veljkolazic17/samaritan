#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkanframebuffer.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanrenderpass.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	void VulkanFramebuffer::Init()
	{
	}

	void VulkanFramebuffer::Shutdown()
	{
	}

	void VulkanFramebuffer::Init(const VulkanCreateArguments& arguments, VulkanRenderpass* renderpass, muint32 width, muint32 height, muint32 attachmentCount, const VkImageView attachments[])
	{
		constexpr muint32 numberOfLayers = 1;

		m_Renderpass = renderpass;
		m_AttachmentCount = attachmentCount;

		if (renderpass == nullptr)
		{
			return;
		}

		//TODO : Change how this is done!
		for (muint16 index = 0; index < attachmentCount; ++index)
		{
			m_Attachments.emplace_back(attachments[index]);
		}

		VkFramebufferCreateInfo framebufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		framebufferCreateInfo.renderPass = renderpass->GetHandle();
		framebufferCreateInfo.attachmentCount = attachmentCount;
		framebufferCreateInfo.pAttachments = m_Attachments.data();
		framebufferCreateInfo.width = width;
		framebufferCreateInfo.height = height;
		framebufferCreateInfo.layers = numberOfLayers;

		VulkanCheckResult(vkCreateFramebuffer(arguments.m_LogicalDevice, &framebufferCreateInfo, arguments.m_Allocator, &m_Handle), "Unable to create framebuffer!");
	}

	void VulkanFramebuffer::Shutdown(const VulkanCreateArguments& arguments)
	{
		vkDestroyFramebuffer(arguments.m_LogicalDevice, m_Handle, arguments.m_Allocator);
		m_Attachments.clear();
		m_AttachmentCount = 0;
		m_Handle = 0;
		m_Renderpass = nullptr;
	}
}

END_NAMESPACE