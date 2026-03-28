#include <engine/graphics/renderer/backend/vulkan/renderpasses/vulkanworldrenderpass.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkancommandbuffer.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
    void VulkanWorldRenderPass::Init(smuint32 width, smuint32 height)
    {
        m_Renderpass.Create
        (
            smVec4(0.0f, 0.0f, static_cast<smfloat32>(width), static_cast<smfloat32>(height)),
            smVec4(1.0f, 1.0f, 1.0f, 1.0f),
            1.f,
            0.f,
            VulkanRenderpassClearFlag::CLEAR_COLOR_BUFFER | VulkanRenderpassClearFlag::CLEAR_DEPTH_BUFFER | VulkanRenderpassClearFlag::CLEAR_STENCIL_BUFFER,
            false,
            false
        );
    }

    void VulkanWorldRenderPass::Destroy()
    {
        m_Renderpass.Destroy();
    }

    void VulkanWorldRenderPass::Begin(VulkanCommandBuffer& cmd, VkFramebuffer framebuffer, smuint32 width, smuint32 height)
    {
        smVec4 renderArea = smVec4{ 0.0f, 0.0f, static_cast<smfloat32>(width), static_cast<smfloat32>(height) };
        m_Renderpass.Begin(renderArea, cmd, framebuffer);
    }

    void VulkanWorldRenderPass::End(VulkanCommandBuffer& cmd)
    {
        m_Renderpass.End(cmd);
    }
}

END_NAMESPACE
