#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanrenderpass.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanrenderpass.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkancommandbuffer.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	void VulkanRenderpass::Create(smVec4 renderArea, smVec4 color, mfloat32 depth, mfloat32 stencil)
	{
		hardAssert(g_VulkanRenderer != nullptr, "Renderer is not set!");

		m_RenderArea = renderArea;
		m_Color = color;
		m_Depth = depth;
		m_Stencil = stencil;

		// Main subpass
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		// Color attachment
		VkAttachmentDescription colorAttachment;
		colorAttachment.format = g_VulkanRenderer->GetVulkanSwapChain().GetImageFormat().format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;      // Do not expect any particular layout before render pass starts.
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  // Transitioned to after the render pass
		colorAttachment.flags = 0;

		VkAttachmentReference colorAattachmentReference;
		colorAattachmentReference.attachment = 0;
		colorAattachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAattachmentReference;

		// Depth attachment, if there is one
		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = g_VulkanRenderer->GetVulkanDevice().m_DepthFormat;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Attachments
		constexpr muint32 attachmentDescriptionCount = 2;
		VkAttachmentDescription attachmentDescriptions[attachmentDescriptionCount] =
		{
			colorAttachment,
			depthAttachment
		};

		// Depth attachment reference
		VkAttachmentReference depthAttachmentReference;
		depthAttachmentReference.attachment = 1;
		depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// TODO: other attachment types (input, resolve, preserve)

		// Depth stencil data.
		subpass.pDepthStencilAttachment = &depthAttachmentReference;

		// Input from a shader
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = 0;

		// Attachments used for multisampling colour attachments
		subpass.pResolveAttachments = 0;

		// Attachments not used in this subpass, but must be preserved for the next.
		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = 0;

		// Render pass dependencies
		VkSubpassDependency dependency;
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dependencyFlags = 0;

		// Render pass create.
		VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassCreateInfo.attachmentCount = attachmentDescriptionCount;
		renderPassCreateInfo.pAttachments = attachmentDescriptions;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependency;
		renderPassCreateInfo.pNext = 0;
		renderPassCreateInfo.flags = 0;

		VulkanCheckResult(vkCreateRenderPass(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, &renderPassCreateInfo, g_VulkanRenderer->GetAllocator(), &m_Handle), "Error creating renderpass!");
	}

	void VulkanRenderpass::Destroy()
	{
		hardAssert(g_VulkanRenderer != nullptr, "Renderer not set!");
		vkDestroyRenderPass(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_Handle, g_VulkanRenderer->GetAllocator());
	}

	void VulkanRenderpass::Begin(smVec4 renderArea, VulkanCommandBuffer& commandBuffer, VkFramebuffer frameBuffer)
	{
		VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		beginInfo.renderPass = m_Handle;
		beginInfo.framebuffer = frameBuffer;
		beginInfo.renderArea.offset.x = renderArea.m_X;
		beginInfo.renderArea.offset.y = renderArea.m_Y;
		beginInfo.renderArea.extent.width = renderArea.m_Z;
		beginInfo.renderArea.extent.height = renderArea.m_W;

		VkClearValue clearValues[2];
		clearValues[0].color.float32[0] = m_Color.m_X;
		clearValues[0].color.float32[1] = m_Color.m_Y;
		clearValues[0].color.float32[2] = m_Color.m_Z;
		clearValues[0].color.float32[3] = m_Color.m_W;
		clearValues[1].depthStencil.depth = m_Depth;
		clearValues[1].depthStencil.stencil = m_Stencil;

		beginInfo.clearValueCount = 2;
		beginInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(commandBuffer.GetHandle(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
		commandBuffer.SetState(VulkanCommandBufferState::COMMAND_BUFFER_STATE_IN_RENDER_PASS);
	}

	void VulkanRenderpass::End(VulkanCommandBuffer& commandBuffer)
	{
		vkCmdEndRenderPass(commandBuffer.GetHandle());
		commandBuffer.SetState(VulkanCommandBufferState::COMMAND_BUFFER_STATE_RECORDING);
	}
}

END_NAMESPACE