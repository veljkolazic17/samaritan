#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>
#include <utils/asserts/assert.hpp>
#include <utils/logger/log.hpp>
#include <engine/memory/memory.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanswapchain.h>

#include <set>

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

        CreateDevice();
        LogInfo(LogChannel::Graphics, "Vulkan device created!");

        //Bad boy code
        m_SwapChain.SetVulkanRenderer(this);
        m_SwapChain.Create(m_VulkanDevice.m_FrameBufferWidth, m_VulkanDevice.m_FrameBufferHeight);
        LogInfo(LogChannel::Graphics, "Vulkan swap chain created!");

        m_MainRenderPass.SetVulkanRenderer(this);
        m_MainRenderPass.Create
        (
            smVec4(0.f, 0.f, (mfloat32)m_VulkanDevice.m_FrameBufferWidth, (mfloat32)m_VulkanDevice.m_FrameBufferHeight),
            smVec4(0.f, 0.f, 0.2f, 0.1f),
            1.f,
            0.f
        );

        CreateCommandBuffers();
    }

    void VulkanRenderer::Shutdown()
    {
        DestroyCommandBuffers();

        m_MainRenderPass.Destroy();

        m_SwapChain.Destroy();

        DestroyDevice();
        LogInfo(LogChannel::Graphics, "Vulkan device destroyed!");

        vkDestroySurfaceKHR(m_Instance, m_Surface, m_Allocator);
        LogInfo(LogChannel::Graphics, "Vulkan surface destroyed!");

        if (m_DebugMessenger)
        {
            PFN_vkDestroyDebugUtilsMessengerEXT procaddr = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
            procaddr(m_Instance, m_DebugMessenger, m_Allocator);
            LogInfo(LogChannel::Graphics, "Vulkan debuger destroyed!");
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

        const VPDRequirements& requirements = m_VulkanDevice.m_VPDRequirments;
        const VPDQueues& vpdQueues = m_VulkanDevice.m_QueuesInfo;

        if (
            (!requirements.m_HasGraphicsQueue || (requirements.m_HasGraphicsQueue && vpdQueues.m_GraphicsIndex != 0xFFFFFFFF)) &&
            (!requirements.m_HasPresentQueue || (requirements.m_HasPresentQueue && vpdQueues.m_PresentIndex != 0xFFFFFFFF)) &&
            (!requirements.m_HasComputeQueue || (requirements.m_HasComputeQueue && vpdQueues.m_ComputeIndex != 0xFFFFFFFF)) &&
            (!requirements.m_HasTransferQueue || (requirements.m_HasTransferQueue && vpdQueues.m_TransferIndex != 0xFFFFFFFF)))
        {
            LogInfo(LogChannel::Graphics, "Device has met specified requirements!");

            //Fuck this is bad code, needs to be changed
            VulkanSwapChainArguments arguments;
            arguments.m_Device = &device;
            arguments.m_Surface = &m_Surface;
            arguments.m_SurfaceCapabilities = &m_Capabilities;
            arguments.m_SurfaceFormats = &m_SurfaceFormats;
            arguments.m_PresentModes = &m_PresentModes;
            VulkanSwapChain::QuerySwapChainSupport(arguments);

            // Check device extensions
            if (requirements.m_DeviceExtensions.size() > 0)
            {
                muint32 extensionCount = 0;
                VulkanCheckResult(vkEnumerateDeviceExtensionProperties(device, 0, &extensionCount, 0), "Error getting extension count!");

                if (extensionCount > 0)
                {
                    std::vector<VkExtensionProperties> extensions(extensionCount);

                    VulkanCheckResult(vkEnumerateDeviceExtensionProperties(device, 0, &extensionCount, extensions.data()), "Error getting extensions!");

                    for (const char* extensionName : requirements.m_DeviceExtensions)
                    {
                        mbool isFound = false;;
                        for (const VkExtensionProperties& extensionProperties : extensions)
                        {
                            if (std::strcmp(extensionName, extensionProperties.extensionName))
                            {
                                isFound = true;
                                break;
                            }
                        }

                        if (!isFound)
                        {
                            LogInfo(LogChannel::Graphics, "Required extension not found: '%s'", extensionName);
                            return false;
                        }
                    }
                }
            }

            if (requirements.m_HasSamplerAnisotropy && !features.samplerAnisotropy)
            {
                LogInfo(LogChannel::Graphics, "Device does not support samplerAnisotropy.");
                return false;
            }

            return true;
        }
        return false;
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

    void VulkanRenderer::CreateVLD()
    {
        muint32 graphicsQueueIndex = m_VulkanDevice.m_QueuesInfo.m_GraphicsIndex;
        muint32 presentQueueIndex = m_VulkanDevice.m_QueuesInfo.m_PresentIndex;
        muint32 transferQueueIndex = m_VulkanDevice.m_QueuesInfo.m_TransferIndex;

        std::set<muint32> queueIndecies = { graphicsQueueIndex, presentQueueIndex, transferQueueIndex };
        
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        for (muint32 queueIndex : queueIndecies) 
        {
            VkDeviceQueueCreateInfo queueCreateInfo;

            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueIndex;
            queueCreateInfo.queueCount = 1;

            //TODO : Maybe problematic
           /* if (queueIndex == graphicsQueueIndex) 
            {
                queueCreateInfo.queueCount = 2;
            }*/
            queueCreateInfo.flags = 0;
            queueCreateInfo.pNext = 0;

            //TODO : This may not work
            mfloat32 queue_priority = 1.0f;
            queueCreateInfo.pQueuePriorities = &queue_priority;

            queueCreateInfos.emplace_back(queueCreateInfo);
        }

        // Request device features.
        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.queueCreateInfoCount = queueIndecies.size();
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = 1;
        const char* extensionNames = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        deviceCreateInfo.ppEnabledExtensionNames = &extensionNames;
        
        VkDevice& logicalDevice = m_VulkanDevice.m_LogicalDevice;

        // Create the device.
        VulkanCheckResult(vkCreateDevice(m_VulkanDevice.m_PhysicalDevice, &deviceCreateInfo, m_Allocator, &logicalDevice), "Error creating Logical Device!");

        LogInfo(LogChannel::Graphics, "Logical Device created!");

        // Get queues.
        vkGetDeviceQueue(logicalDevice, graphicsQueueIndex, 0, &m_VulkanDevice.m_GraphicsQueue);
        vkGetDeviceQueue(logicalDevice, presentQueueIndex, 0, &m_VulkanDevice.m_PresentQueue);
        vkGetDeviceQueue(logicalDevice, transferQueueIndex, 0, &m_VulkanDevice.m_TransferQueue);

        LogInfo(LogChannel::Graphics, "Queues obtained!");

        CreateCommandPool();
    }

    void VulkanRenderer::CreateDevice()
    {
        SelectVPD();
        CreateVLD();
    }

    void VulkanRenderer::DestroyDevice()
    {
        // Destroy logical device
        DestroyCommandPool();

        vkDestroyDevice(m_VulkanDevice.m_LogicalDevice, m_Allocator);
        LogInfo(LogChannel::Graphics, "Logical Device destroyed!");

        Zero(&m_Capabilities, sizeof(m_Capabilities));
        m_VulkanDevice.m_Properties;

        //Don't need to do this!
        m_VulkanDevice.m_VPDRequirments.m_DeviceExtensions.clear();

        m_VulkanDevice.m_QueuesInfo.m_GraphicsIndex = 0xFFFFFFFF;
        m_VulkanDevice.m_QueuesInfo.m_PresentIndex = 0xFFFFFFFF;
        m_VulkanDevice.m_QueuesInfo.m_ComputeIndex = 0xFFFFFFFF;
        m_VulkanDevice.m_QueuesInfo.m_TransferIndex = 0xFFFFFFFF;

        //WTF should I event do this
        Memory::mmfree(m_Allocator, false);
    }

    void VulkanRenderer::CreateCommandBuffers()
    {
        constexpr mbool isPrimary = true;

        if(m_GraphicsCommandBuffers.size())
        {
            m_GraphicsCommandBuffers.resize(m_SwapChain.GetImages().size());
        }

        for (VulkanCommandBuffer& commandBuffer : m_GraphicsCommandBuffers)
        {
            if (commandBuffer.GetCommandBuffer())
            {
                commandBuffer.FreeBuffer(m_VulkanDevice.m_LogicalDevice, m_VulkanDevice.m_GraphicsCommandPool);
            }

            commandBuffer.AllocateBuffer(m_VulkanDevice.m_LogicalDevice, m_VulkanDevice.m_GraphicsCommandPool, isPrimary);
        }
        LogDebug(LogChannel::Graphics, "Created command buffers");
    }

    void VulkanRenderer::DestroyCommandBuffers()
    {
        for (VulkanCommandBuffer& commandBuffer : m_GraphicsCommandBuffers)
        {
            if (commandBuffer.GetCommandBuffer())
            {
                commandBuffer.FreeBuffer(m_VulkanDevice.m_LogicalDevice, m_VulkanDevice.m_GraphicsCommandPool);
            }
        }
        m_GraphicsCommandBuffers.clear();
    }

    void VulkanRenderer::CreateCommandPool()
    {
        VkCommandPoolCreateInfo poolCcreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        poolCcreateInfo.queueFamilyIndex = m_VulkanDevice.m_QueuesInfo.m_GraphicsIndex;
        poolCcreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        VulkanCheckResult(vkCreateCommandPool(m_VulkanDevice.m_LogicalDevice, &poolCcreateInfo, m_Allocator, &m_VulkanDevice.m_GraphicsCommandPool), "Failed to create Graphics Command Pool!");
        LogInfo(LogChannel::Graphics, "Graphics command pool created.");
    }

    void VulkanRenderer::DestroyCommandPool()
    {
        vkDestroyCommandPool(m_VulkanDevice.m_LogicalDevice, m_VulkanDevice.m_GraphicsCommandPool, m_Allocator);
        LogInfo(LogChannel::Graphics, "Destroying command pools.");
    }

	void VulkanRenderer::Resize(muint32 width, muint32 height)
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