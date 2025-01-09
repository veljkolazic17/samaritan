#include <engine/graphics/renderer/backend/vulkan/vulkanshadermodule.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkanobjectshader.hpp>

#include <engine/memory/memory.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanobjectshader.hpp>
#include <engine/filesystem/bfile.hpp>

BEGIN_NAMESPACE

namespace Graphics
{

	mbool VulkanShaderModule::CreateVulkanShaderModule
	(
		mcstring name,
		mcstring type,
		VkShaderStageFlagBits stageFlags,
		muint32 stageIndex,
        VulkanShaderStage shaderStages[],
        VkDevice logicalDevice,
        VkAllocationCallbacks* allocator
	)
	{
        //TODO : Change this
        std::string fileName = "C:/Users/veljk/source/repos/DoomerBoomer/DoomerBoomer/MeowEngine/bin/assets/shaders/" + std::string(name) + "." + std::string(type) + ".spv";

        //Zero(&shaderStages[stageIndex].m_CrateInfo, sizeof(VkShaderModuleCreateInfo));
        shaderStages[stageIndex].m_CrateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

        // Obtain file handle.
        //TODO : Should Open return bool?
        BFile file;
        //FUCKED : Why do I need to add binary when using binary files :|
        file.Open(fileName.c_str(), std::ios::in | std::ios::binary);
        muint64 fileLength = file.Length();

        // Read the entire file as binary.
        //Allocate memory for this
        void* buffer = (void*)gpAllocRenderer(fileLength);
        file.Read(buffer);

        shaderStages[stageIndex].m_CrateInfo.codeSize = fileLength;
        shaderStages[stageIndex].m_CrateInfo.pCode = (muint32*)buffer;

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
        Zero(&shaderStages[stageIndex].m_ShaderStageCreateInfo, sizeof(VkPipelineShaderStageCreateInfo));
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