#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>

BEGIN_NAMESPACE

//forward declares always do in seperate namesace
namespace Graphics
{
	struct VulkanShaderStage;
}

namespace Graphics
{
	class VulkanShaderModule
	{
	public:
		static smbool CreateVulkanShaderModule(
			smcstring name,
			smcstring type,
			VkShaderStageFlagBits stageFlags,
			smuint32 stageIndex,
			VulkanShaderStage shaderStages[],
			VkDevice logicalDevice,
			VkAllocationCallbacks* allocator
		);
	};
}

END_NAMESPACE