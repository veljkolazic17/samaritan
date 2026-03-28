#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/renderpasses/vulkanrenderpass.hpp>
#include <engine/graphics/renderer/backend/vulkan/renderpasses/vulkanrendertotexture.hpp>

#include <engine/graphics/renderer/renderertype.inl>

BEGIN_NAMESPACE

namespace Graphics
{
    class VulkanCommandBuffer;

    class VulkanPickingRenderPass
    {
    public:
        void Init(smuint32 width, smuint32 height);
        void Destroy();

        smuint32 Execute(smuint32 mouseX, smuint32 mouseY, const smMat4& projection, const smMat4& view, const GeometryData* drawCalls, smuint32 drawCallCount);

        SM_INLINE VulkanRenderpass& GetRenderpass() { return m_Renderpass; }
        SM_INLINE VkRenderPass GetHandle() { return m_Renderpass.GetHandle(); }

    private:
        void CreateRenderpass();

        VulkanRenderpass m_Renderpass;
        VulkanRenderToTexture m_RenderToTexture;

        smuint32 m_Width = 0;
        smuint32 m_Height = 0;
    };
}

END_NAMESPACE
