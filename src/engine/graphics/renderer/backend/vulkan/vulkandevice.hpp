#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>
#include <objecttemplates/shutdownable.hpp>

#include <vector>

BEGIN_NAMESPACE

namespace Graphics
{
	struct VPDRequirements 
	{
		mbool m_HasGraphicsQueue = false;
		mbool m_HasPresentQueue = false;
		mbool m_HasComputeQueue = false;
		mbool m_HasTransferQueue = false;

		mbool m_HasSamplerAnisotropy = false;
		mbool m_IsDiscrete = false;

		std::vector<const char*> m_DeviceExtensions;
	};

	struct VPDQueues 
	{
		muint32 m_GraphicsIndex = 0xFFFFFFFF;
		muint32 m_PresentIndex = 0xFFFFFFFF;
		muint32 m_ComputeIndex = 0xFFFFFFFF;
		muint32 m_TransferIndex = 0xFFFFFFFF;
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

		muint32 m_FrameBufferWidth = 0;
		muint32 m_FrameBufferHeight = 0;

		VkCommandPool m_GraphicsCommandPool;

		//SUS
		static SM_INLINE muint32 FindMemoryIndex(VkPhysicalDevice physicalDevice, muint32 typeFilter, muint32 propertyFlags)
		{
			VkPhysicalDeviceMemoryProperties memoryProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
			for (muint32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
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