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

	void VulkanFramebuffer::Init(const VulkanCreateArguments& arguments, VulkanRenderpass* renderpass, smuint32 width, smuint32 height, smuint32 attachmentCount, const VkImageView attachments[])
	{
		constexpr smuint32 numberOfLayers = 1;

		if (renderpass == nullptr)
		{
			return;
		}

		m_Renderpass = renderpass;
		m_AttachmentCount = attachmentCount;

		//TODO : Change how this is done!
		m_Attachments.clear();
		for (smuint16 index = 0; index < attachmentCount; ++index)
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