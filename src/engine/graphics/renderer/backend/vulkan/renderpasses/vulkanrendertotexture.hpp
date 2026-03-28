#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanimage.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbuffer.hpp>

BEGIN_NAMESPACE
namespace Graphics
{
class VulkanRenderToTexture
{
public:
    struct PixelInfo
    {
        smuint32 m_ObjectInfo;
    };

    void Init(smuint32 windowWidth, smuint32 windowHeight, VkRenderPass renderpass);
    void Destroy();
    smuint32 ReadPixel(smuint32 x, smuint32 y);

    SM_INLINE VkFramebuffer   GetFramebuffer()    { return m_Framebuffer; }
    SM_INLINE VulkanImage&    GetColorImage()     { return m_ColorImage; }
    SM_INLINE VulkanBuffer&   GetStagingBuffer()  { return m_StagingBuffer; }
private:
    void InitFramebuffer(VkRenderPass renderpass);

    VkFramebuffer  m_Framebuffer;
    VulkanImage    m_ColorImage;
    VulkanImage    m_DepthImage;
    VulkanBuffer   m_StagingBuffer;

    smuint32 m_Width;
    smuint32 m_Heigth;
};
}

END_NAMESPACE