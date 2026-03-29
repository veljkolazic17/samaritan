#include <engine/graphics/renderer/backend/vulkan/renderpasses/vulkanpickingrenderpass.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkancommandbuffer.hpp>

#include <engine/graphics/systems/shadersystem.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanshader.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
    void VulkanPickingRenderPass::Init(smuint32 width, smuint32 height)
    {
        m_Width = width;
        m_Height = height;

        CreateRenderpass();
        m_RenderToTexture.Init(width, height, m_Renderpass.GetHandle());
    }

    void VulkanPickingRenderPass::Destroy()
    {
        m_RenderToTexture.Destroy();
        m_Renderpass.Destroy();
    }

    void VulkanPickingRenderPass::CreateRenderpass()
    {
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkAttachmentDescription attachments[2];

        VkAttachmentDescription& colorAttachment = attachments[0];
        colorAttachment = {};
        colorAttachment.format = VK_FORMAT_R32_UINT;//redner entity id into image
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        VkAttachmentReference colorRef;
        colorRef.attachment = 0;
        colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;

        VkAttachmentDescription& depthAttachment = attachments[1];
        depthAttachment = {};
        depthAttachment.format = g_VulkanRenderer->GetVulkanDevice().m_DepthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthRef;
        depthRef.attachment = 1;
        depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        subpass.pDepthStencilAttachment = &depthRef;

        VkSubpassDependency deps[2];
        deps[0] = {};
        deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        deps[0].dstSubpass = 0;
        deps[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        deps[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        deps[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        deps[1] = {};
        deps[1].srcSubpass = VK_SUBPASS_EXTERNAL;
        deps[1].dstSubpass = 0;
        deps[1].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        deps[1].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        deps[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo rpInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        rpInfo.attachmentCount = 2;
        rpInfo.pAttachments = attachments;
        rpInfo.subpassCount = 1;
        rpInfo.pSubpasses = &subpass;
        rpInfo.dependencyCount = 2;
        rpInfo.pDependencies = deps;

        VkRenderPass rawHandle;
        VulkanCheckResult(vkCreateRenderPass(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, &rpInfo, g_VulkanRenderer->GetAllocator(), &rawHandle), "Failed to create picking renderpass!");
        m_Renderpass.SetHandle(rawHandle);
    }

    smuint32 VulkanPickingRenderPass::Execute(smuint32 mouseX, smuint32 mouseY, const smMat4& projection, const smMat4& view, const GeometryData* drawCalls, smuint32 drawCallCount)
    {
        const ResourceHandle<Shader>& shaderHandle = smShaderSystem().GetShader("PickingShader");
        if (!shaderHandle.IsValid())
        {
            return 0;
        }

        Shader* shader = shaderHandle.GetResource();
        VulkanShader* vkShader = static_cast<VulkanShader*>(shader->m_InternalData);

        // Set projection ane view into shader UBO memory
        g_VulkanRenderer->ShaderBindGlobals(shader);
        g_VulkanRenderer->ShaderSetUniform(shader, shader->m_UniformLookupTable.at("projection"), &projection);
        g_VulkanRenderer->ShaderSetUniform(shader, shader->m_UniformLookupTable.at("view"), &view);

        VulkanCommandBuffer cmd;
        cmd.BeginSingleUseBuffer(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, g_VulkanRenderer->GetVulkanDevice().m_GraphicsCommandPool);

        // Update and bind global descriptor set using the picking command buffer
        g_VulkanRenderer->ShaderApplyGlobals(shader, cmd.GetHandle(), 0);

        // Begin picking renderpass
        VkClearValue clearValues[2];
        clearValues[0].color.uint32[0] = std::numeric_limits<unsigned int>::max();
        clearValues[1].depthStencil = { 1.0f, 0 };

        VkRenderPassBeginInfo rpBegin = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        rpBegin.renderPass = m_Renderpass.GetHandle();
        rpBegin.framebuffer = m_RenderToTexture.GetFramebuffer();
        rpBegin.renderArea.offset = { 0, 0 };
        rpBegin.renderArea.extent = { m_Width, m_Height };
        rpBegin.clearValueCount = 2;
        rpBegin.pClearValues = clearValues;
        vkCmdBeginRenderPass(cmd.GetHandle(), &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

        vkShader->m_VulkanPipeline.Bind(&cmd, VK_PIPELINE_BIND_POINT_GRAPHICS);

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = static_cast<smfloat32>(m_Height);
        viewport.width = static_cast<smfloat32>(m_Width);
        viewport.height = -static_cast<smfloat32>(m_Height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmd.GetHandle(), 0, 1, &viewport);

        VkRect2D scissor = 
        { 
            { 0, 0 }, 
            { m_Width, m_Height }
        };

        vkCmdSetScissor(cmd.GetHandle(), 0, 1, &scissor);
        vkCmdSetLineWidth(cmd.GetHandle(), 1.0f);

        // Draw each object
        VkPipelineLayout layout = vkShader->m_VulkanPipeline.GetPipelineLayout();
        const ShaderUniform& modelUniform = shader->m_UniformLookupTable.at("model");
        const ShaderUniform& objectIdUniform = shader->m_UniformLookupTable.at("object_id");

        for (smuint32 i = 0; i < drawCallCount; ++i)
        {
            const GeometryData& call = drawCalls[i];
            if (!call.m_Geometry || call.m_Geometry->m_Id == SM_INVALID_ID) continue;

            Vulkan::Types::GeometryData& geomData = g_VulkanRenderer->GetGeometryData(call.m_Geometry->m_Handle);

            vkCmdPushConstants(cmd.GetHandle(), layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, static_cast<smuint32>(modelUniform.m_Offset), static_cast<smuint32>(modelUniform.m_Size), &call.m_Model);
            vkCmdPushConstants(cmd.GetHandle(), layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, static_cast<smuint32>(objectIdUniform.m_Offset), static_cast<smuint32>(objectIdUniform.m_Size), &call.m_ObjectId);

            VkDeviceSize vertexOffset = geomData.m_VertexBufferOffset;
            vkCmdBindVertexBuffers(cmd.GetHandle(), 0, 1, &g_VulkanRenderer->GetVertexBuffer().GetHandle(), &vertexOffset);

            if (geomData.m_IndexCount > 0)
            {
                vkCmdBindIndexBuffer(cmd.GetHandle(), g_VulkanRenderer->GetIndexBuffer().GetHandle(), geomData.m_IndexBufferOffset, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(cmd.GetHandle(), geomData.m_IndexCount, 1, 0, 0, 0);
            }
            else
            {
                vkCmdDraw(cmd.GetHandle(), geomData.m_VertexCount, 1, 0, 0);
            }
        }

        vkCmdEndRenderPass(cmd.GetHandle());

        //Reading the buffer
        VkBufferImageCopy copyRegion = {};
        copyRegion.bufferOffset = 0;
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageOffset = { static_cast<smint32>(mouseX), static_cast<smint32>(mouseY), 0 };
        copyRegion.imageExtent = { 1, 1, 1 };
        vkCmdCopyImageToBuffer(cmd.GetHandle(), m_RenderToTexture.GetColorImage().GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_RenderToTexture.GetStagingBuffer().GetHandle(), 1, &copyRegion);

        // Barrier: transfer write -> host read
        VkMemoryBarrier barrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER };
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
        vkCmdPipelineBarrier(cmd.GetHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 1, &barrier, 0, nullptr, 0, nullptr);

        // Submit and wait
        cmd.EndSingleUseBuffer(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, g_VulkanRenderer->GetVulkanDevice().m_GraphicsCommandPool, g_VulkanRenderer->GetVulkanDevice().m_GraphicsQueue);

        // Read entity ID
        void* mapped = m_RenderToTexture.GetStagingBuffer().LockMemory(0, sizeof(smuint32), 0);
        smuint32 objectId = *static_cast<smuint32*>(mapped);
        m_RenderToTexture.GetStagingBuffer().UnlockMemory();

        return objectId;
    }
}

END_NAMESPACE
