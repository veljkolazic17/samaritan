#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>
#include <utils/asserts/assert.hpp>
#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
	void VulkanRenderer::Init()
	{
        // Setup Vulkan instance.
        VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.pApplicationName = "samaritan app";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "samaritan engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = 0;
        createInfo.ppEnabledExtensionNames = 0;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = 0;

        VkResult result = vkCreateInstance(&createInfo, m_Allocator, &m_Instance);
        if (result != VK_SUCCESS) {
            hardAssert(false, "Error Initializing Vulkan Instance !");
        }

        LogInfo(LogChannel::Graphics, "Vulkan Initialized");
	}

	void VulkanRenderer::Shutdown()
	{

	}

	void VulkanRenderer::Resize(muint32 width, muint32 heigth)
	{

	}

	void VulkanRenderer::BeginFrame(Time time)
	{

	}

	void VulkanRenderer::EndFrame(Time time)
	{

	}
}

END_NAMESPACE