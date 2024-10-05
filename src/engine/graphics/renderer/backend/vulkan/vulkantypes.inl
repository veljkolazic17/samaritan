#pragma once
#include <defines.hpp>
#include <utils/asserts/assert.hpp>
#include <vulkan/vulkan.h>


#define VulkanCheckResult(result, message)					\
if (result != VK_SUCCESS) {									\
hardAssert(false, message);									\
}

#define SM_MAX_FRAMES_IN_FLIGHT								2

BEGIN_NAMESPACE

namespace Graphics
{
	struct VulkanCreateArguments
	{
		VulkanCreateArguments(const VkDevice& logicalDevice, const VkAllocationCallbacks* allocator) 
		:	m_LogicalDevice(logicalDevice),
			m_Allocator(allocator)
		{}

		const VkDevice& m_LogicalDevice;
		const VkAllocationCallbacks* m_Allocator;
	};
}

END_NAMESPACE