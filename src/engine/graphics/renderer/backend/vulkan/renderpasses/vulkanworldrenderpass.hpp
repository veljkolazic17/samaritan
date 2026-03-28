#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/renderpasses/vulkanrenderpass.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
    class VulkanCommandBuffer;

    class VulkanWorldRenderPass
    {
    public:
        void Init(smuint32 width, smuint32 height);
        void Destroy();
        void Begin(VulkanCommandBuffer& cmd, VkFramebuffer framebuffer, smuint32 width, smuint32 height);
        void End(VulkanCommandBuffer& cmd);

        SM_INLINE VulkanRenderpass& GetRenderpass() { return m_Renderpass; }
        SM_INLINE VkRenderPass GetHandle() { return m_Renderpass.GetHandle(); }

    private:
        VulkanRenderpass m_Renderpass;
    };
}

END_NAMESPACE
