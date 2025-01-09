#include <engine/graphics/renderer/backend/vulkan/vulkanobjectshader.hpp>

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
        //TODO : Descriptors

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

        // TODO: Desciptor set layouts.

        // Stages
        // NOTE: Should match the number of shader->stages.
        VkPipelineShaderStageCreateInfo stageCreateInfos[OBJECT_SHADER_STAGE_COUNT];
        Zero(stageCreateInfos, sizeof(stageCreateInfos));

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
                0,
                0,
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
	}

	void VulkanObjectShader::Destroy(VkDevice logicalDevice, VkAllocationCallbacks* allocator)
	{
        m_VulkanPipeline.Destroy(logicalDevice, allocator);

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
}

END_NAMESPACE