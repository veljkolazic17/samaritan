#pragma once
#include <defines.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkanimage.hpp>

BEGIN_NAMESPACE
namespace Graphics
{
	struct VulkanTextureData
	{
		VulkanImage m_Image;
		VkSampler m_Sampler;
	};

}
END_NAMESPACE