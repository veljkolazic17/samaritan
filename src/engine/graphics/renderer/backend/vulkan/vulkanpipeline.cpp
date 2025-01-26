#include <engine/graphics/renderer/backend/vulkan/vulkanpipeline.hpp>

#include <engine/memory/memory.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>
#include <utils/asserts/assert.hpp>
#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
    mbool VulkanPipeline::Create
    (
        VulkanRenderpass* renderpass,
        muint32 attributeCount,
        VkVertexInputAttributeDescription* attributes,
        muint32 descriptorSetLayoutCount,
        VkDescriptorSetLayout* descriptorSetLayouts,
        muint32 stageCount,
        VkPipelineShaderStageCreateInfo* stages,
        VkViewport viewport,
        VkRect2D scissor,
        mbool isWireframe,
        VkDevice logicalDevice,
        VkAllocationCallbacks* allocator
    )
    {
        // Viewport state
        VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizerCreateInfo.depthClampEnable = VK_FALSE;
        rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerCreateInfo.polygonMode = isWireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
        rasterizerCreateInfo.lineWidth = 1.0f;
        rasterizerCreateInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
        rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
        rasterizerCreateInfo.depthBiasClamp = 0.0f;
        rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

        // Multisampling.
        VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
        multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisamplingCreateInfo.minSampleShading = 1.0f;
        multisamplingCreateInfo.pSampleMask = 0;
        multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;

        // Depth and stencil testing.
        VkPipelineDepthStencilStateCreateInfo depthStencil = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
        smZero(&colorBlendAttachmentState, sizeof(VkPipelineColorBlendAttachmentState));
        colorBlendAttachmentState.blendEnable = VK_TRUE;
        colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;

        // Dynamic state
        constexpr muint32 dynamicStateCount = 3;
        VkDynamicState dynamic_states[dynamicStateCount] = 
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_LINE_WIDTH 
        };

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
        dynamicStateCreateInfo.pDynamicStates = dynamic_states;

        // Vertex input
        VkVertexInputBindingDescription bindingDescription;
        bindingDescription.binding = 0;  // Binding index
        bindingDescription.stride = sizeof(smVec3);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;  // Move to next data entry for each vertex.

        // Attributes
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = attributeCount;
        vertexInputInfo.pVertexAttributeDescriptions = attributes;

        // Input assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

        // Push constants
        VkPushConstantRange pushConstant;
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstant.offset = sizeof(smMat4) * 0;
        pushConstant.size = sizeof(smMat4) * 2;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;

        // Descriptor set layouts
        pipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayoutCount;
        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts;

        // Create the pipeline layout.
        VulkanCheckResult
        (
            vkCreatePipelineLayout
            (
                logicalDevice,
                &pipelineLayoutCreateInfo,
                allocator,
                &m_PipelineLayout
            ),
            "Could not create VulkanPipeline!"
        );

        // Pipeline create
        VkGraphicsPipelineCreateInfo pipelineCreateInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipelineCreateInfo.stageCount = stageCount;
        pipelineCreateInfo.pStages = stages;
        pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssembly;

        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
        pipelineCreateInfo.pDepthStencilState = &depthStencil;
        pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
        pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        pipelineCreateInfo.pTessellationState = 0;

        pipelineCreateInfo.layout = m_PipelineLayout;

        hardAssert(renderpass != nullptr, "Renderpass was nullptr!");

        pipelineCreateInfo.renderPass = renderpass->GetHandle();
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;

        VkResult result = vkCreateGraphicsPipelines
        (
            logicalDevice,
            VK_NULL_HANDLE,
            1,
            &pipelineCreateInfo,
            allocator,
            &m_Handle
        );

        if (!Vulkan::Utils::IsResultSuccess(result)) 
        {
            hardAssert(false, "vkCreateGraphicsPipelines failed: %s", Vulkan::Utils::ResultToString(result));
        }

        LogInfo(LogChannel::Graphics, "Graphics pipeline created!");

        return true;
    }
    
    void VulkanPipeline::Destroy(VkDevice logicalDevice, VkAllocationCallbacks* allocator)
    {
        // Destroy pipeline
        if (m_Handle != 0) 
        {
            vkDestroyPipeline(logicalDevice, m_Handle, allocator);
            m_Handle = 0;
        }

        // Destroy layout
        if (m_PipelineLayout) 
        {
            vkDestroyPipelineLayout(logicalDevice, m_PipelineLayout, allocator);
            m_PipelineLayout = 0;
        }
    }

    void VulkanPipeline::Bind(VulkanCommandBuffer* commandBuffer, VkPipelineBindPoint bindPoint)
    {
        hardAssert(commandBuffer != nullptr, "Command buffer is nullptr!");
        vkCmdBindPipeline(commandBuffer->GetHandle(), bindPoint, m_Handle);
    }
}

END_NAMESPACE