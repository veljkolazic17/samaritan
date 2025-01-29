#include <engine/graphics/renderer/backend/vulkan/vulkanobjectshader.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanshadermodule.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanrenderpass.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkancommandbuffer.hpp>

#include <math/vector.hpp>
#include <engine/memory/memory.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	void VulkanObjectShader::Create(VulkanRenderpass* renderpass, VkDevice logicalDevice, VkAllocationCallbacks* allocator, mfloat32 framebufferWidth, mfloat32 framebufferHeight)
	{
		// Shader module init per stage.
		muint8 stageTypeNames[OBJECT_SHADER_STAGE_COUNT][5] = { "vert", "frag" };
		VkShaderStageFlagBits stageTypes[OBJECT_SHADER_STAGE_COUNT] = 
		{ 
			VK_SHADER_STAGE_VERTEX_BIT, 
			VK_SHADER_STAGE_FRAGMENT_BIT 
		};

		for (muint16 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) 
		{
			if (!VulkanShaderModule::CreateVulkanShaderModule(BUILTIN_SHADER_NAME_OBJECT, (mcstring)stageTypeNames[i], stageTypes[i], i, m_Stages, logicalDevice, allocator))
			{
				hardAssert(false, "Unable to create %s shader module for '%s'.", stageTypeNames[i], BUILTIN_SHADER_NAME_OBJECT);
				return;
			}
		}
        //Descriptors used for creating global ubo

        const muint32 imageCount = g_VulkanRenderer->GetVulkanSwapChain().GetImageCount();

        VkDescriptorSetLayoutBinding globalUboLayoutBinding;
        globalUboLayoutBinding.binding = 0;
        globalUboLayoutBinding.descriptorCount = 1;
        globalUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        globalUboLayoutBinding.pImmutableSamplers = 0;
        globalUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo globalLayoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        globalLayoutInfo.bindingCount = 1;
        globalLayoutInfo.pBindings = &globalUboLayoutBinding;

        VulkanCheckResult
        (
            vkCreateDescriptorSetLayout(logicalDevice, &globalLayoutInfo, allocator, &m_GlobalDescriptorSetLayout),
            "Could not create decriptor set layout!"
        );

        // Global descriptor pool used for global items such as view and projection matrix.
        VkDescriptorPoolSize globalPoolSize;
        globalPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        globalPoolSize.descriptorCount = imageCount;

        VkDescriptorPoolCreateInfo globalPoolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        globalPoolInfo.poolSizeCount = 1;
        globalPoolInfo.pPoolSizes = &globalPoolSize;
        globalPoolInfo.maxSets = imageCount;

        VulkanCheckResult
        (
            vkCreateDescriptorPool(logicalDevice, &globalPoolInfo, allocator, &m_GlobalDescriptorPool),
            "Could not create descriptor pool"
        );

        // Pipeline creation
        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = framebufferWidth;
        viewport.height = framebufferHeight;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Scissor
        VkRect2D scissor;
        scissor.offset.x = scissor.offset.y = 0;
        scissor.extent.width = framebufferWidth;
        scissor.extent.height = framebufferHeight;

        // Attributes
        muint32 offset = 0;
        constexpr muint32 attributeCount = 1;
        VkVertexInputAttributeDescription attributeDescriptions[attributeCount];
        // Position
        VkFormat formats[attributeCount] = 
        {
            VK_FORMAT_R32G32B32_SFLOAT
        };

        muint64 sizes[attributeCount] = 
        {
            sizeof(smVec3)
        };

        for (muint32 i = 0; i < attributeCount; ++i)
        {
            attributeDescriptions[i].binding = 0;   // binding index - should match binding desc
            attributeDescriptions[i].location = i;  // attrib location
            attributeDescriptions[i].format = formats[i];
            attributeDescriptions[i].offset = offset;
            offset += sizes[i];
        }

        // Desciptor set layouts
        constexpr muint32 descriptorSetLayoutCount = 1;
        VkDescriptorSetLayout layouts[descriptorSetLayoutCount] = 
        {
            m_GlobalDescriptorSetLayout 
        };

        // Stages
        // NOTE: Should match the number of shader->stages.
        VkPipelineShaderStageCreateInfo stageCreateInfos[OBJECT_SHADER_STAGE_COUNT];
        smZero(stageCreateInfos, sizeof(stageCreateInfos));

        for (muint32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) 
        {
            //stageCreateInfos[i].sType = m_Stages[i].m_ShaderStageCreateInfo.sType;
            stageCreateInfos[i] = m_Stages[i].m_ShaderStageCreateInfo;
        }

        constexpr mbool isUsingWireframe = false;

        if (
            !m_VulkanPipeline.Create
            (
                renderpass,
                attributeCount,
                attributeDescriptions,
                descriptorSetLayoutCount,
                layouts,
                OBJECT_SHADER_STAGE_COUNT,
                stageCreateInfos,
                viewport,
                scissor,
                isUsingWireframe,
                logicalDevice,
                allocator
            )
        )
        {
            hardAssert(false, "Failed to load graphics pipeline for object shader.");
        }

        // Create uniform buffer.
        m_GlobalUniformBuffer.Create
        (
            sizeof(GlobalUniformObject),
            (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT),
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            true
        );

        // Allocate global descriptor sets. We use 3 same here because we want to put same layout
        constexpr muint32 maxImageCount = 3;
        VkDescriptorSetLayout globalLayouts[maxImageCount];
        smZero(globalLayouts, maxImageCount * sizeof(VkDescriptorSetLayout));
        for (muint8 i = 0; i < imageCount; ++i)
        {
            globalLayouts[i] = m_GlobalDescriptorSetLayout;
        }

        VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        allocInfo.descriptorPool = m_GlobalDescriptorPool;
        allocInfo.descriptorSetCount = imageCount;
        allocInfo.pSetLayouts = globalLayouts;
        VulkanCheckResult
        (
            vkAllocateDescriptorSets(logicalDevice, &allocInfo, m_GlobalDescriptorSets),
            "Could not allocate descriptor sets!"
        );
	}

	void VulkanObjectShader::Destroy(VkDevice logicalDevice, VkAllocationCallbacks* allocator)
	{
        // Destroy uniform buffer.
        m_GlobalUniformBuffer.Destroy();

        // Destroy pipeline.
        m_VulkanPipeline.Destroy(logicalDevice, allocator);

        // Destroy global descriptor pool.
        vkDestroyDescriptorPool(logicalDevice, m_GlobalDescriptorPool, allocator);

        // Destroy descriptor set layouts.
        vkDestroyDescriptorSetLayout(logicalDevice, m_GlobalDescriptorSetLayout, allocator);

        // Destroy shader modules.
        for (muint32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) 
        {
            vkDestroyShaderModule(logicalDevice, m_Stages[i].m_Handle, allocator);
            m_Stages[i].m_Handle = 0;
        }
	}

	void VulkanObjectShader::Use(VulkanCommandBuffer* commandBuffer)
	{
        m_VulkanPipeline.Bind(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS);
	}

    void VulkanObjectShader::UpdateGlobalState()
    {
        VkCommandBuffer& commandBuffer = g_VulkanRenderer->GetGraphicsCommandBuffers()[g_VulkanRenderer->GetImageIndex()].GetHandle();
        VkDescriptorSet globalDescriptor = m_GlobalDescriptorSets[g_VulkanRenderer->GetImageIndex()];

        // Configure the descriptors for the given index.
        muint32 range = sizeof(GlobalUniformObject);
        muint64 offset = 0;

        // Copy data to buffer
        m_GlobalUniformBuffer.LoadData(offset, range, 0, &m_GlobalUbo);

        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = m_GlobalUniformBuffer.GetHandle();
        bufferInfo.offset = offset;
        bufferInfo.range = range;

        // Update descriptor sets.
        VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        descriptorWrite.dstSet = m_GlobalDescriptorSets[g_VulkanRenderer->GetImageIndex()];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, 1, &descriptorWrite, 0, 0);

        // Bind the global descriptor set to be updated.
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline.GetPipelineLayout(), 0, 1, &globalDescriptor, 0, 0);
    }

    void VulkanObjectShader::UpdateModel(smMat4 model)
    {
        VkCommandBuffer commandBuffer = g_VulkanRenderer->GetGraphicsCommandBuffers()[g_VulkanRenderer->GetImageIndex()].GetHandle();
        vkCmdPushConstants(commandBuffer, m_VulkanPipeline.GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(smMat4), &model);
    }

    muint32 VulkanObjectShader::AcquireObjectShaderResources()
	{
        ++m_ObjectUniformBufferIndex;
        constexpr muint32 frameNumber = 3;
        VulkanObjectShaderObjectState* objectState = &m_ObjectStates[m_ObjectUniformBufferIndex];
        for (muint32 i = 0; i < SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT; ++i) 
        {
            for (muint32 j = 0; j < frameNumber; ++j)
            {
                objectState->m_DescriptorStates[i].m_Generation[j] = SM_INVALID_ID;
            }
        }

        // Allocate descriptor sets.
        VkDescriptorSetLayout layouts[frameNumber] =
        {
        	m_DescriptorSetLayout,
        	m_DescriptorSetLayout,
        	m_DescriptorSetLayout
        };

        VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.descriptorSetCount = frameNumber;
        allocInfo.pSetLayouts = layouts;
        VkResult result = vkAllocateDescriptorSets(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, &allocInfo, objectState->m_DescriptorSet);
        if (result != VK_SUCCESS) 
        {
            softAssert(false, "Could not allocate descriptor sets!");
            return SM_INVALID_ID;
        }

        return m_ObjectUniformBufferIndex;
	}

    void VulkanObjectShader::ReleaseObjectShaderResources(muint32 objectID)
	{
        VulkanObjectShaderObjectState* objectState = &m_ObjectStates[objectID];
        constexpr muint32 descriptorSetCount = 3;
        // Release object descriptor sets.
        VkResult result = vkFreeDescriptorSets(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_DescriptorPool, descriptorSetCount, objectState->m_DescriptorStates;
        if (result != VK_SUCCESS) 
        {
            softAssert(false, "Could not free descriptor sets!");
        }

        constexpr muint32 frameCount = 3;
        for (muint32 i = 0; i < SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT; ++i) 
        {
            for (muint32 j = 0; j < frameCount; ++j)
            {
                objectState->m_DescriptorStates[i].m_Generation[j] = SM_INVALID_ID;
            }
        }
	}
}

END_NAMESPACE