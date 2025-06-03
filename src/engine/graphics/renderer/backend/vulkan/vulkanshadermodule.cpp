#include <engine/graphics/renderer/backend/vulkan/vulkanshadermodule.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkanobjectshader.hpp>

#include <engine/memory/memory.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanobjectshader.hpp>
#include <engine/filesystem/bfile.hpp>

#include <format>

BEGIN_NAMESPACE

namespace Graphics
{

	smbool VulkanShaderModule::CreateVulkanShaderModule
	(
		smcstring name,
		smcstring type,
		VkShaderStageFlagBits stageFlags,
		smuint32 stageIndex,
        VulkanShaderStage shaderStages[],
        VkDevice logicalDevice,
        VkAllocationCallbacks* allocator
	)
	{
        std::string fileName = std::format("{}/{}.{}.spv", BUILTIN_SHADERS_PATH ,name, type);

        shaderStages[stageIndex].m_CrateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

        // Obtain file handle.
        //TODO : Should Open return bool?
        BFile file;
        file.Open(fileName.c_str(), std::ios::in);
        smuint64 fileLength = file.Length();

        // Read the entire file as binary.
        //Allocate memory for this
        void* buffer = (void*)gpAllocRenderer(fileLength);
        file.Read(buffer);

        shaderStages[stageIndex].m_CrateInfo.codeSize = fileLength;
        shaderStages[stageIndex].m_CrateInfo.pCode = (smuint32*)buffer;

        // Close the file.
        file.Close();

        VulkanCheckResult
        (
            vkCreateShaderModule
            (
                logicalDevice,
                &shaderStages[stageIndex].m_CrateInfo,
                allocator,
                &shaderStages[stageIndex].m_Handle
            ),
            "Failed to create shader module"
        );

        // Shader stage info
        smZero(&shaderStages[stageIndex].m_ShaderStageCreateInfo, sizeof(VkPipelineShaderStageCreateInfo));
        shaderStages[stageIndex].m_ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[stageIndex].m_ShaderStageCreateInfo.stage = stageFlags;
        shaderStages[stageIndex].m_ShaderStageCreateInfo.module = shaderStages[stageIndex].m_Handle;
        shaderStages[stageIndex].m_ShaderStageCreateInfo.pName = "main";

        if (buffer != nullptr) 
        {
            gpFreeRenderer((void*)buffer, fileLength);
            buffer = nullptr;
        }

        return true;
	}
}

END_NAMESPACE