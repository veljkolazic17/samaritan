#ifdef TARGET_WIN
#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	void VulkanRenderer::GetPlatformExtensions(std::vector<const char*>& platfromExtensions)
	{
		platfromExtensions.emplace_back("VK_KHR_win32_surface");
	}
}

END_NAMESPACE

#endif