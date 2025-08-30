#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.hpp>
#include <objecttemplates/shutdownable.hpp>

#include <vector>

BEGIN_NAMESPACE

namespace Graphics
{
	struct VPDRequirements 
	{
		smbool m_HasGraphicsQueue = false;
		smbool m_HasPresentQueue = false;
		smbool m_HasComputeQueue = false;
		smbool m_HasTransferQueue = false;

		smbool m_HasSamplerAnisotropy = false;
		smbool m_IsDiscrete = false;

		std::vector<const char*> m_DeviceExtensions;
	};

	struct VPDQueues 
	{
		smuint32 m_GraphicsIndex = 0xFFFFFFFF;
		smuint32 m_PresentIndex = 0xFFFFFFFF;
		smuint32 m_ComputeIndex = 0xFFFFFFFF;
		smuint32 m_TransferIndex = 0xFFFFFFFF;
	};

	struct VulkanDevice
	{
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_LogicalDevice;

		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceFeatures m_Features;
		VkPhysicalDeviceMemoryProperties m_Memory;

		VPDQueues m_QueuesInfo;
		VPDRequirements m_VPDRequirments;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		VkQueue m_TransferQueue;

		VkFormat m_DepthFormat;

		smuint32 m_FrameBufferWidth = 0;
		smuint32 m_FrameBufferHeight = 0;

		VkCommandPool m_GraphicsCommandPool;

		//SUS
		static SM_INLINE smuint32 FindMemoryIndex(VkPhysicalDevice physicalDevice, smuint32 typeFilter, smuint32 propertyFlags)
		{
			VkPhysicalDeviceMemoryProperties memoryProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
			for (smuint32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
			{
				if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & typeFilter) == propertyFlags)
				{
					return i;
				}
			}
			return -1;
		}
	};
}

END_NAMESPACE