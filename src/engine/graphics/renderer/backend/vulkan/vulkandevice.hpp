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


	struct VulkanDevice : public Shutdownable
	{
		void Init() override;
		void Shutdown() override;

	
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_LogicalDevice;

		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceFeatures m_Features;
		VkPhysicalDeviceMemoryProperties m_Memory;

		VPDQueues m_QueuesInfo;
		VPDRequirements m_VPDRequirments;
	};
}

END_NAMESPACE