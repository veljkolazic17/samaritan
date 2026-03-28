#include <engine/graphics/renderer/backend/vulkan/renderpasses/vulkanrendertotexture.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkanimage.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbuffer.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>

BEGIN_NAMESPACE
namespace Graphics
{

void VulkanRenderToTexture::Init(smuint32 windowWidth, smuint32 windowHeight, VkRenderPass renderpass)
{
    m_Width  = windowWidth;
    m_Heigth = windowHeight;
    InitFramebuffer(renderpass);
}

void VulkanRenderToTexture::Destroy()
{
    VkDevice device = g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice;
    VkAllocationCallbacks* allocator = g_VulkanRenderer->GetAllocator();

    m_StagingBuffer.Destroy();
    VulkanImage::DeleteImage(device, allocator, m_DepthImage);
    VulkanImage::DeleteImage(device, allocator, m_ColorImage);
    vkDestroyFramebuffer(device, m_Framebuffer, allocator);
}

void VulkanRenderToTexture::InitFramebuffer(VkRenderPass renderpass)
{
    VkFormat colorFormat = VK_FORMAT_R32_UINT;
    VkFormat depthFormat = g_VulkanRenderer->GetVulkanDevice().m_DepthFormat;

    // Color image
    VulkanImage::CreateImage(
        g_VulkanRenderer->GetVulkanDevice().m_PhysicalDevice,
        g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice,
        g_VulkanRenderer->GetAllocator(),
        VK_IMAGE_TYPE_2D,
        m_Width,
        m_Heigth,
        colorFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true,
        VK_IMAGE_ASPECT_COLOR_BIT,
        &m_ColorImage
    );

    // Depth image
    VulkanImage::CreateImage(
        g_VulkanRenderer->GetVulkanDevice().m_PhysicalDevice,
        g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice,
        g_VulkanRenderer->GetAllocator(),
        VK_IMAGE_TYPE_2D,
        m_Width,
        m_Heigth,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &m_DepthImage
    );

    // Staging buffer for CPU read
    m_StagingBuffer.Create(
        m_Width * m_Heigth * sizeof(smuint32),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        true
    );
    VkImageView attachments[] = { m_ColorImage.GetImageView(), m_DepthImage.GetImageView() };

    VkFramebufferCreateInfo fbInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    fbInfo.renderPass = renderpass;
    fbInfo.attachmentCount = 2;
    fbInfo.pAttachments = attachments;
    fbInfo.width = m_Width;
    fbInfo.height = m_Heigth;
    fbInfo.layers = 1;

    VulkanCheckResult(vkCreateFramebuffer(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, &fbInfo, g_VulkanRenderer->GetAllocator(), &m_Framebuffer), "Failed to create picking framebuffer!");
}

smuint32 VulkanRenderToTexture::ReadPixel(smuint32 x, smuint32 y)
{
    return 0;
}

}
END_NAMESPACE
