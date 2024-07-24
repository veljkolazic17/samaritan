#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>
#include <utils/asserts/assert.hpp>
#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
    VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
    {
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        {
            LogError(LogChannel::Graphics, callbackData->pMessage);
        }
        break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        {
            LogWarning(LogChannel::Graphics, callbackData->pMessage);
        }
        break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        {
            LogInfo(LogChannel::Graphics, callbackData->pMessage);
        }
        break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        {
            //TODO : Should this be debug?
            LogDebug(LogChannel::Graphics, callbackData->pMessage);
        }
        break;
        }
        return VK_FALSE;
    }

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

        std::vector<const char*> extensions;
        extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
        GetPlatformExtensions(extensions);
#ifdef DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();


#ifdef DEBUG
        std::vector<const char*> layers;
        layers.emplace_back("VK_LAYER_KHRONOS_validation");

        muint32 availableLayerCount = 0;
        VulkanCheckResult(vkEnumerateInstanceLayerProperties(&availableLayerCount, 0), "Error enumerating layer properties");
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        VulkanCheckResult(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()), "Error enumerating layer properties");
        
        for (const char* requiredLayerName : layers)
        {
            mbool isLayerPresent = false;
            for (const VkLayerProperties& layerProperties : availableLayers)
            {
                if (std::strcmp(requiredLayerName, layerProperties.layerName))
                {
                    isLayerPresent = true;
                    break;
                }
            }
            hardAssert(isLayerPresent, "Layer Not Found");
        }

        createInfo.enabledLayerCount = layers.size();
        createInfo.ppEnabledLayerNames = layers.data();

#else
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = 0;
#endif
        VulkanCheckResult(vkCreateInstance(&createInfo, m_Allocator, &m_Instance), "Failed creating Vulkan instance");
        LogInfo(LogChannel::Graphics, "Vulkan Initialized");


#ifdef DEBUG
        muint32 logTypes = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debugCreateInfo.messageSeverity = logTypes;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debugCreateInfo.pfnUserCallback = vkDebugCallback;

        PFN_vkCreateDebugUtilsMessengerEXT procaddr = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
        hardAssert(procaddr, "Wrong function name!");

        VulkanCheckResult(procaddr(m_Instance, &debugCreateInfo, m_Allocator, &m_DebugMessenger), "Could not create debugger!");
        LogInfo(LogChannel::Graphics, "Vulkan debugger created!");
#endif
        CreatePlatformSurface();
        LogInfo(LogChannel::Graphics, "Vulkan surface created!");

        m_VulkanDevice.Init();
        LogInfo(LogChannel::Graphics, "Vulkan device created!");
	}

	void VulkanRenderer::Shutdown()
    {
        if (m_DebugMessenger) 
        {
            PFN_vkDestroyDebugUtilsMessengerEXT procaddr = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
            procaddr(m_Instance, m_DebugMessenger, m_Allocator);
        }
        vkDestroyInstance(m_Instance, m_Allocator);
        LogInfo(LogChannel::Graphics, "Vulkan instance destroyed!");
	}

    mbool VulkanRenderer::CheckDeviceRequerments
    (
        const VkPhysicalDevice& device,
        const VkPhysicalDeviceProperties& properties,
        const VkPhysicalDeviceFeatures& features,
        const VkPhysicalDeviceMemoryProperties& memory
    )
    {
        if (m_VulkanDevice.m_VPDRequirments.m_IsDiscrete && properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            hardAssert(false, "Supporting only descrete GPUs");
        }

        muint32 queuesFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queuesFamilyCount, 0);
        std::vector<VkQueueFamilyProperties> queuesFamily(queuesFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queuesFamilyCount, queuesFamily.data());

        muint8 minimumTransferScore = 255;

        muint8 indexCounter = 0;
        for (const VkQueueFamilyProperties& familyProperties : queuesFamily)
        {
            muint8 transferScore = 0;

            // Graphics queue?
            if (familyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                m_VulkanDevice.m_QueuesInfo.m_GraphicsIndex = indexCounter;
                ++transferScore;
            }

            // Compute queue?
            if (familyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) 
            {
                m_VulkanDevice.m_QueuesInfo.m_ComputeIndex = indexCounter;
                ++transferScore;
            }

            // Transfer queue?
            if (familyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) 
            {
                if (transferScore <= minimumTransferScore) 
                {
                    minimumTransferScore = transferScore;
                    m_VulkanDevice.m_QueuesInfo.m_TransferIndex = indexCounter;
                }
            }

            // Present queue?
            VkBool32 isPresnetSupported = VK_FALSE;
            VulkanCheckResult(vkGetPhysicalDeviceSurfaceSupportKHR(device, indexCounter, m_Surface, &isPresnetSupported), "Error Getting Device Presetn Support!");
            if (isPresnetSupported)
            {
                m_VulkanDevice.m_QueuesInfo.m_PresentIndex = indexCounter;
            }

            ++indexCounter;
        }
        //TODO : More check here

    }

    void VulkanRenderer::SelectVPD()
    {
        muint32 deviceCount = 0;
        mbool deviceSelected = false;

        VulkanCheckResult(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, 0), "Error getting device number!");
        if (deviceCount == 0) 
        {
            hardAssert(false, "No devices support Vulkan!");
        }

        std::vector<VkPhysicalDevice> vpds(deviceCount);
        VulkanCheckResult(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, vpds.data()), "Error getting devices!");

        for (const VkPhysicalDevice& vpd : vpds)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(vpd, &properties);

            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(vpd, &features);

            VkPhysicalDeviceMemoryProperties memory;
            vkGetPhysicalDeviceMemoryProperties(vpd, &memory);

            VPDRequirements requirements;
            requirements.m_HasGraphicsQueue = true;
            requirements.m_HasPresentQueue = true;
            requirements.m_HasTransferQueue = true;
            requirements.m_HasSamplerAnisotropy = true;
            requirements.m_IsDiscrete = true;

            requirements.m_DeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

            if (CheckDeviceRequerments(vpd, properties, features, memory))
            {


                m_VulkanDevice.m_PhysicalDevice = std::move(vpd);
                m_VulkanDevice.m_Properties = std::move(properties);
                m_VulkanDevice.m_Features = std::move(features);
                m_VulkanDevice.m_Memory = std::move(memory);
                
                deviceSelected = true;
                break;
            }
        }
        hardAssert(deviceSelected, "No Vulkan device selected!");
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