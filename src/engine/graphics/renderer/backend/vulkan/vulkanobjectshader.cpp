#include <engine/graphics/renderer/backend/vulkan/vulkanobjectshader.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanshadermodule.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanrenderpass.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkancommandbuffer.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantexturedata.hpp>
#include <engine/graphics/systems/texturesystem.hpp>
#include <engine/resources/graphics/material.hpp>

#include <math/vector.hpp>
#include <engine/memory/memory.hpp>
#include <engine/Engine.hpp>



BEGIN_NAMESPACE
namespace Graphics
{
	void VulkanObjectShader::Create(VulkanRenderpass* renderpass, VkDevice logicalDevice, VkAllocationCallbacks* allocator, smfloat32 framebufferWidth, smfloat32 framebufferHeight)
	{
		// Shader module init per stage.
		smuint8 stageTypeNames[SM_OBJECT_SHADER_STAGE_COUNT][5] = { "vert", "frag" };
		VkShaderStageFlagBits stageTypes[SM_OBJECT_SHADER_STAGE_COUNT] = 
		{ 
			VK_SHADER_STAGE_VERTEX_BIT, 
			VK_SHADER_STAGE_FRAGMENT_BIT 
		};

		for (smuint16 i = 0; i < SM_OBJECT_SHADER_STAGE_COUNT; ++i) 
		{
			if (!VulkanShaderModule::CreateVulkanShaderModule(SM_BUILTIN_SHADER_NAME_OBJECT, (smcstring)stageTypeNames[i], stageTypes[i], i, m_Stages, logicalDevice, allocator))
			{
				hardAssert(false, "Unable to create %s shader module for '%s'.", stageTypeNames[i], BUILTIN_SHADER_NAME_OBJECT);
				return;
			}
		}
#pragma region GLOBAL_UBO_CREATION
        //Descriptors used for creating global ubo
        const smuint32 imageCount = g_VulkanRenderer->GetVulkanSwapChain().GetImageCount();

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
#pragma endregion
#pragma region OBJECT_UBO_CREATION
        m_SamplerUsages[0] = TextureUsageType::TEXTURE_USAGE_MAP_DIFFUSE;

        VkDescriptorType descriptorTypes[SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT] = 
        {
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // Binding 0 - uniform buffer
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // Binding 1 - Diffuse sampler layout.
        };

        VkDescriptorSetLayoutBinding bindings[SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT];
        smZero(&bindings, sizeof(VkDescriptorSetLayoutBinding) * SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT);
        for (smuint32 i = 0; i < SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT; ++i) 
        {
            bindings[i].binding = i;
            bindings[i].descriptorCount = 1;
            bindings[i].descriptorType = descriptorTypes[i];
            bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        layoutInfo.bindingCount = SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT;
        layoutInfo.pBindings = bindings;
        VulkanCheckResult
		(
            vkCreateDescriptorSetLayout(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, &layoutInfo, nullptr, &m_DescriptorSetLayout),
			"Failed to create descriptor set layout"
        );

        // Object descriptor pool
        constexpr smuint32 objectPoolSizesSize = 2;
        VkDescriptorPoolSize objectPoolSizes[objectPoolSizesSize];
        // The first section will be used for uniform buffers
        objectPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        objectPoolSizes[0].descriptorCount = SM_VULKAN_OBJECT_MAX_OBJECT_COUNT;
        // The second section will be used for image samplers.
        objectPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        objectPoolSizes[1].descriptorCount = SM_VULKAN_OBJECT_SHADER_SAMPLER_COUNT * SM_VULKAN_OBJECT_MAX_OBJECT_COUNT;
        VkDescriptorPoolCreateInfo objectPoolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        objectPoolInfo.poolSizeCount = objectPoolSizesSize;
        objectPoolInfo.pPoolSizes = objectPoolSizes;
        objectPoolInfo.maxSets = SM_VULKAN_OBJECT_MAX_OBJECT_COUNT;
        // Create object descriptor pool.
        VulkanCheckResult
		(
            vkCreateDescriptorPool(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, &objectPoolInfo, g_VulkanRenderer->GetAllocator(), &m_DescriptorPool),
			"Could not create descriptor pool"
        );
#pragma endregion

        // Pipeline creation
        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = framebufferHeight;
        viewport.width = framebufferWidth;
        viewport.height = - framebufferHeight;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Scissor
        VkRect2D scissor;
        scissor.offset.x = scissor.offset.y = 0;
        scissor.extent.width = framebufferWidth;
        scissor.extent.height = framebufferHeight;

        // Attributes
        smuint32 offset = 0;
        constexpr smuint32 attributeCount = 2;
        VkVertexInputAttributeDescription attributeDescriptions[attributeCount];
        // Position
        VkFormat formats[attributeCount] = 
        {
            VK_FORMAT_R32G32B32_SFLOAT,
            VK_FORMAT_R32G32_SFLOAT
        };

        smuint64 sizes[attributeCount] = 
        {
			sizeof(smVec3), //pos
            sizeof(smVec2),  //texture pos
        };

        for (smuint32 i = 0; i < attributeCount; ++i)
        {
            attributeDescriptions[i].binding = 0;   // binding index - should match binding desc
            attributeDescriptions[i].location = i;  // attrib location
            attributeDescriptions[i].format = formats[i];
            attributeDescriptions[i].offset = offset;
            offset += sizes[i];
        }

        // Desciptor set layouts
        constexpr smuint32 descriptorSetLayoutCount = 2;
        VkDescriptorSetLayout layouts[descriptorSetLayoutCount] = 
        {
            m_GlobalDescriptorSetLayout,
            m_DescriptorSetLayout 
        };

        // Stages
        // NOTE: Should match the number of shader->stages.
        VkPipelineShaderStageCreateInfo stageCreateInfos[SM_OBJECT_SHADER_STAGE_COUNT];
        smZero(stageCreateInfos, sizeof(stageCreateInfos));

        for (smuint32 i = 0; i < SM_OBJECT_SHADER_STAGE_COUNT; ++i) 
        {
            //stageCreateInfos[i].sType = m_Stages[i].m_ShaderStageCreateInfo.sType;
            stageCreateInfos[i] = m_Stages[i].m_ShaderStageCreateInfo;
        }

        constexpr smbool isUsingWireframe = false;

        if (
            !m_VulkanPipeline.Create
            (
                renderpass,
                attributeCount,
                attributeDescriptions,
                descriptorSetLayoutCount,
                layouts,
                SM_OBJECT_SHADER_STAGE_COUNT,
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
        constexpr smuint32 maxImageCount = 3;
        VkDescriptorSetLayout globalLayouts[maxImageCount];
        smZero(globalLayouts, maxImageCount * sizeof(VkDescriptorSetLayout));
        for (smuint8 i = 0; i < imageCount; ++i)
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

        m_ObjectUniformBuffer.Create
        (
            sizeof(ObjectUniformObject) * SM_VULKAN_OBJECT_MAX_OBJECT_COUNT,
			static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT),
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			true
        );
	}

	void VulkanObjectShader::Destroy(VkDevice logicalDevice, VkAllocationCallbacks* allocator)
	{
        // Destroy uniform buffer.
        m_GlobalUniformBuffer.Destroy();
        m_ObjectUniformBuffer.Destroy();

        // Destroy pipeline.
        m_VulkanPipeline.Destroy(logicalDevice, allocator);

        // Destroy global descriptor pool.
        vkDestroyDescriptorPool(logicalDevice, m_GlobalDescriptorPool, allocator);

        // Destroy descriptor set layouts.
        vkDestroyDescriptorSetLayout(logicalDevice, m_GlobalDescriptorSetLayout, allocator);

        // Destroy shader modules.
        for (smuint32 i = 0; i < SM_OBJECT_SHADER_STAGE_COUNT; ++i) 
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
        const smuint32 imageIndex = g_VulkanRenderer->GetImageIndex();

        VkCommandBuffer& commandBuffer = g_VulkanRenderer->GetGraphicsCommandBuffers()[imageIndex].GetHandle();
        VkDescriptorSet globalDescriptor = m_GlobalDescriptorSets[imageIndex];

        // Configure the descriptors for the given index.
        smuint32 range = sizeof(GlobalUniformObject);
        smuint64 offset = 0;

        // Copy data to buffer
        m_GlobalUniformBuffer.LoadData(offset, range, 0, &m_GlobalUbo);

        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = m_GlobalUniformBuffer.GetHandle();
        bufferInfo.offset = offset;
        bufferInfo.range = range;

        // Update descriptor sets.
        VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        descriptorWrite.dstSet = m_GlobalDescriptorSets[imageIndex];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, 1, &descriptorWrite, 0, 0);

        // Bind the global descriptor set to be updated.
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline.GetPipelineLayout(), 0, 1, &globalDescriptor, 0, 0);
    }

    void VulkanObjectShader::UpdateModel(GeometryData data)
    {
        const smuint32 imageIndex = g_VulkanRenderer->GetImageIndex();

        VkCommandBuffer commandBuffer = g_VulkanRenderer->GetGraphicsCommandBuffers()[imageIndex].GetHandle();
        vkCmdPushConstants(commandBuffer, m_VulkanPipeline.GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(smMat4), &data.m_Model);

        const Material* const material = data.m_Material;
        if (material == nullptr)
        {
            softAssert(false, "Trying to update nullpt as material!");
            return;
        }

        // Obtain material data.
        VulkanObjectShaderObjectState& objectState = m_ObjectStates[material->m_InternalID];
        VkDescriptorSet objectDescriptorSet = objectState.m_DescriptorSet[imageIndex];

        // TODO: [GRAPHICS] if needs update
        VkWriteDescriptorSet descriptorWrites[SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT];
        smZero(descriptorWrites, sizeof(VkWriteDescriptorSet) * SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT);
        smuint32 descriptorCount = 0;
        smuint32 descriptorIndex = 0;

        // Descriptor 0 - Uniform buffer
        smuint32 range = sizeof(ObjectUniformObject);
        smuint64 offset = sizeof(ObjectUniformObject) * material->m_InternalID;
        ObjectUniformObject obo;
        obo.m_Color = material->m_DiffuseColor;

        // Load the data into the buffer.
        m_ObjectUniformBuffer.LoadData(offset, range, 0, &obo);
        // Only do this if the descriptor has not yet been updated.
        smuint32& globalUboGeneration = objectState.m_DescriptorStates[descriptorIndex].m_Generation[imageIndex];
        if (globalUboGeneration == SM_INVALID_ID || globalUboGeneration != material->m_Generation)
		{
            VkDescriptorBufferInfo bufferInfo;
            bufferInfo.buffer = m_ObjectUniformBuffer.GetHandle();
            bufferInfo.offset = offset;
            bufferInfo.range = range;

            VkWriteDescriptorSet descriptor = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptor.dstSet = objectDescriptorSet;
            descriptor.dstBinding = descriptorIndex;
            descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor.descriptorCount = 1;
            descriptor.pBufferInfo = &bufferInfo;
            descriptorWrites[descriptorCount] = descriptor;
            descriptorCount++;
            // Update the frame generation. In this case it is only needed once since this is a buffer.
            globalUboGeneration = material->m_Generation;
        }
        ++descriptorIndex;
        //Samplers.
        constexpr smuint32 samplerCount = 1;
        VkDescriptorImageInfo imageInfos[1];
        for (smuint32 samplerIndex = 0; samplerIndex < samplerCount; ++samplerIndex)
        {
            Texture* texture = nullptr;
            switch (m_SamplerUsages[samplerIndex])
            {
            case TextureUsageType::TEXTURE_USAGE_MAP_DIFFUSE:
            {
                texture = material->m_DiffuseMap.m_Texture;
                break;
            }
            default:
            {
                softAssert(false, "Sampler with unknown useage");
                return;
            }
            }

            smuint32& descriptorGeneration = objectState.m_DescriptorStates[descriptorIndex].m_Generation[imageIndex];
            smuint32& descriptorID = objectState.m_DescriptorStates[descriptorIndex].m_ID[imageIndex];

            if (texture == nullptr || texture->m_Generation == SM_INVALID_ID)
            {
                texture = &smTextureSystem().GetDefaultTexture();
                descriptorGeneration = SM_INVALID_ID;
            }

            // Check if the descriptor needs updating first.
            if (texture != nullptr && (descriptorID != texture->m_ID || descriptorGeneration != texture->m_Generation || descriptorGeneration == SM_INVALID_ID)) 
            {
                VulkanTextureData* textureData = static_cast<VulkanTextureData*>(texture->m_Data);
                // Assign view and sampler.
                imageInfos[samplerIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfos[samplerIndex].imageView = textureData->m_Image.GetImageView();
                imageInfos[samplerIndex].sampler = textureData->m_Sampler;

                VkWriteDescriptorSet descriptor = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                descriptor.dstSet = objectDescriptorSet;
                descriptor.dstBinding = descriptorIndex;
                descriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptor.descriptorCount = 1;
                descriptor.pImageInfo = &imageInfos[samplerIndex];
                descriptorWrites[descriptorCount] = descriptor;
                ++descriptorCount;
                // Sync frame generation if not using a default texture.
                if (texture->m_Generation != SM_INVALID_ID) 
                {
                    descriptorGeneration = texture->m_Generation;
                    descriptorID = texture->m_ID;
                }
                ++descriptorIndex;
            }
        }
        if (descriptorCount > 0)
        {
            vkUpdateDescriptorSets(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, descriptorCount, descriptorWrites, 0, 0);
        }
        // Bind the descriptor set to be updated, or in case the shader changed.
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanPipeline.GetPipelineLayout(), 1, 1, &objectDescriptorSet, 0, 0);
    }

    smuint32 VulkanObjectShader::AcquireObjectShaderResources(Material* outMaterial)
	{
        constexpr smuint32 frameNumber = 3;
        VulkanObjectShaderObjectState* objectState = &m_ObjectStates[m_ObjectUniformBufferIndex];
        for (smuint32 i = 0; i < SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT; ++i) 
        {
            for (smuint32 j = 0; j < frameNumber; ++j)
            {
                objectState->m_DescriptorStates[i].m_Generation[j] = SM_INVALID_ID;
                objectState->m_DescriptorStates[i].m_ID[j] = SM_INVALID_ID;
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

        //TODO : [CRITICAL][GRAPHICS] Check if uniform buffer should increase as internal id
        outMaterial->m_InternalID = m_ObjectUniformBufferIndex;
        return ++m_ObjectUniformBufferIndex;


	}

    void VulkanObjectShader::ReleaseObjectShaderResources(Material* material)
	{
        if (material == nullptr)
        {
            hardAssert(false, "Trying to release nullptr as material!");
        }

        VulkanObjectShaderObjectState* objectState = &m_ObjectStates[material->m_InternalID];
        constexpr smuint32 descriptorSetCount = 3;
        // Release object descriptor sets.
        VkResult result = vkFreeDescriptorSets(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, m_DescriptorPool, descriptorSetCount, &objectState->m_DescriptorSet[material->m_InternalID]);
        if (result != VK_SUCCESS) 
        {
            softAssert(false, "Could not free descriptor sets!");
        }

        constexpr smuint32 frameCount = 3;
        for (smuint32 i = 0; i < SM_VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT; ++i) 
        {
            for (smuint32 j = 0; j < frameCount; ++j)
            {
                objectState->m_DescriptorStates[i].m_Generation[j] = SM_INVALID_ID;
                objectState->m_DescriptorStates[i].m_ID[j] = SM_INVALID_ID;
            }
        }

        material->m_Generation = SM_INVALID_ID;
	}
}

END_NAMESPACE