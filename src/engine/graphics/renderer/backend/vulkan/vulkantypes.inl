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

    namespace Vulkan::Utils
    {
        smbool IsResultSuccess(VkResult result);
		const char* ResultToString(VkResult result);
    }

	namespace Vulkan::Types
	{
		struct GeometryData 
		{
			smuint32 m_Id = SM_INVALID_ID;
			smuint32 m_Generation = SM_INVALID_ID;
			smuint32 m_VertexCount = 0;
			smuint32 m_VertexSize = 0;
			smuint32 m_VertexBufferOffset = 0;
			smuint32 m_IndexCount = 0;
			smuint32 m_IndexSize = 0;
			smuint32 m_IndexBufferOffset = 0;
		};
	}
}

END_NAMESPACE