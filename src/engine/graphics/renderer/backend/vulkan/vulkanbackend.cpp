#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>

#include <engine/graphics/renderer/backend/vulkan/vulkantexturedata.hpp>
#include <engine/graphics/systems/materialsystem.hpp>
#include <engine/memory/memory.hpp>
#include <engine/resources/graphics/material.hpp>

#include <set>
#include <utils/asserts/assert.hpp>
#include <utils/logger/log.hpp>

//TODO : [FUCKED][GRAPHICS] changing screen size makes objects look weird

BEGIN_NAMESPACE

namespace Graphics
{
    VulkanRenderer* g_VulkanRenderer = nullptr;

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
        g_VulkanRenderer = this;

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

        smuint32 availableLayerCount = 0;
        VulkanCheckResult(vkEnumerateInstanceLayerProperties(&availableLayerCount, 0), "Error enumerating layer properties");
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        VulkanCheckResult(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()), "Error enumerating layer properties");

        for (const char* requiredLayerName : layers)
        {
            smbool isLayerPresent = false;
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
        smuint32 logTypes = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

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

        m_VulkanDevice.m_FrameBufferWidth = 1280;
        m_VulkanDevice.m_FrameBufferHeight = 720;

        m_SwapChain.Create(m_VulkanDevice.m_FrameBufferWidth, m_VulkanDevice.m_FrameBufferHeight);
        LogInfo(LogChannel::Graphics, "Vulkan swap chain created!");

        m_MainRenderPass.Create
        (
            smVec4(0.0f, 0.0f, static_cast<smfloat32>(m_VulkanDevice.m_FrameBufferWidth), static_cast<smfloat32>(m_VulkanDevice.m_FrameBufferHeight)),
            smVec4(0.5f, 0.5f, 0.5f, 1.0f),
            1.f,
            0.f
        );

        // Regenerate framebuffers
        m_SwapChain.RegenerateFramebuffers();
        LogInfo(LogChannel::Graphics, "Creating framebuffers for the first time!");

        CreateCommandBuffers();

        CreateSyncObjects();

        //TODO : Why is renderpass pointer argument???
        m_ObjectShader.Create(&m_MainRenderPass, m_VulkanDevice.m_LogicalDevice, m_Allocator, m_VulkanDevice.m_FrameBufferWidth, m_VulkanDevice.m_FrameBufferHeight);

        CreateBuffers();

        for (smuint32 i = 0; i < SM_VULKAN_MAX_GEOMETRY_COUNT; ++i)
        {
            m_GeometryData[i].m_Id = SM_INVALID_ID;
        }
    }

    void VulkanRenderer::Shutdown()
    {
        DestroyBuffers();

        m_ObjectShader.Destroy(m_VulkanDevice.m_LogicalDevice, m_Allocator);

        DestroySyncObjects();

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

    smbool VulkanRenderer::CheckDeviceRequerments
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

        smuint32 queuesFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queuesFamilyCount, 0);
        std::vector<VkQueueFamilyProperties> queuesFamily(queuesFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queuesFamilyCount, queuesFamily.data());

        smuint8 minimumTransferScore = 255;

        smuint8 indexCounter = 0;
        for (const VkQueueFamilyProperties& familyProperties : queuesFamily)
        {
            smuint8 transferScore = 0;

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

            //TODO : Fuck this is bad code, needs to be changed
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
                smuint32 extensionCount = 0;
                VulkanCheckResult(vkEnumerateDeviceExtensionProperties(device, 0, &extensionCount, 0), "Error getting extension count!");

                if (extensionCount > 0)
                {
                    std::vector<VkExtensionProperties> extensions(extensionCount);

                    VulkanCheckResult(vkEnumerateDeviceExtensionProperties(device, 0, &extensionCount, extensions.data()), "Error getting extensions!");

                    for (const char* extensionName : requirements.m_DeviceExtensions)
                    {
                        smbool isFound = false;;
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
        smuint32 deviceCount = 0;
        smbool deviceSelected = false;

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
        smuint32 graphicsQueueIndex = m_VulkanDevice.m_QueuesInfo.m_GraphicsIndex;
        smuint32 presentQueueIndex = m_VulkanDevice.m_QueuesInfo.m_PresentIndex;
        smuint32 transferQueueIndex = m_VulkanDevice.m_QueuesInfo.m_TransferIndex;

        std::set<smuint32> queueIndecies = { graphicsQueueIndex, presentQueueIndex, transferQueueIndex };
        
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        for (smuint32 queueIndex : queueIndecies) 
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
            smfloat32 queuePriority = 1.0f;
            queueCreateInfo.pQueuePriorities = &queuePriority;

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

        smZero(&m_Capabilities, sizeof(m_Capabilities));
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
        constexpr smbool isPrimary = true;

        if(m_GraphicsCommandBuffers.size() == 0)
        {
            m_GraphicsCommandBuffers.resize(m_SwapChain.GetImages().size());
        }

        for (VulkanCommandBuffer& commandBuffer : m_GraphicsCommandBuffers)
        {
            if (commandBuffer.GetHandle())
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
            if (commandBuffer.GetHandle())
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

    // proting from C code
    void VulkanRenderer::CreateSyncObjects()
    {
        // We want to create signaled because our application will wait for fence first time it is created
        // so we just marked it as "rendered"
        constexpr smbool isSignaled = true;

        m_ImageAvailableSemaphores.resize(SM_MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(SM_MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.reserve(SM_MAX_FRAMES_IN_FLIGHT);

        for (smuint8 index = 0; index < SM_MAX_FRAMES_IN_FLIGHT; ++index)
        {
            VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
            VulkanCheckResult(vkCreateSemaphore(m_VulkanDevice.m_LogicalDevice, &semaphoreCreateInfo, m_Allocator, &m_ImageAvailableSemaphores[index]), "Cannot create vksemaphore!");
            VulkanCheckResult(vkCreateSemaphore(m_VulkanDevice.m_LogicalDevice, &semaphoreCreateInfo, m_Allocator, &m_RenderFinishedSemaphores[index]), "Cannot create vksemaphore!");
            VulkanFence fence;
            fence.Create(m_VulkanDevice.m_LogicalDevice, isSignaled, m_Allocator);
            m_InFlightFences.emplace_back(fence);
        }

        m_ImagesInFlight.resize(m_SwapChain.GetImages().size());
        LogInfo(LogChannel::Graphics, "Created sync objects.");
    }

    void VulkanRenderer::DestroySyncObjects()
    {
        for (smuint8 index = 0; index < SM_MAX_FRAMES_IN_FLIGHT; ++index)
        {
            vkDestroySemaphore(m_VulkanDevice.m_LogicalDevice, m_ImageAvailableSemaphores[index], m_Allocator);
            vkDestroySemaphore(m_VulkanDevice.m_LogicalDevice, m_RenderFinishedSemaphores[index], m_Allocator);

            VulkanFence& fence = m_InFlightFences[index];
            fence.Destroy(m_VulkanDevice.m_LogicalDevice, m_Allocator);
        }
        m_ImageAvailableSemaphores.clear();
        m_RenderFinishedSemaphores.clear();
        m_ImagesInFlight.clear();
        LogInfo(LogChannel::Graphics, "Destroyed sync objects.");
    }

    void VulkanRenderer::DestroyCommandPool()
    {
        vkDestroyCommandPool(m_VulkanDevice.m_LogicalDevice, m_VulkanDevice.m_GraphicsCommandPool, m_Allocator);
        LogInfo(LogChannel::Graphics, "Destroying command pools.");
    }

	void VulkanRenderer::Resize(smuint32 width, smuint32 height)
	{
        m_VulkanDevice.m_FrameBufferWidth = width;
        m_VulkanDevice.m_FrameBufferHeight = height;
        ++m_FrameBuffferGeneration;
	}

	smbool VulkanRenderer::BeginFrame(Time time)
	{
        constexpr smbool isSingleUse = false;
        constexpr smbool isRenderpassContinue = false;
        constexpr smbool isSimultaneousUse = false;

        // Check if recreating swapchain
        if (m_IsRecreatingSwapchain) 
        {
            VkResult result = vkDeviceWaitIdle(m_VulkanDevice.m_LogicalDevice);
            if (!Vulkan::Utils::IsResultSuccess(result))
            {
                softAssert(false, "VulkanRenderer::BeginFrame vkDeviceWaitIdle failed: '%s'", Vulkan::Utils::ResultToString(result));
                return false;
            }
            LogInfo(LogChannel::Graphics,"Recreating swapchain!");
            return false;
        }

        //Check if there was a resize event; if so recreate swapchain
        if (m_FrameBuffferLastGeneration != m_FrameBuffferGeneration)
        {
            VkResult result = vkDeviceWaitIdle(m_VulkanDevice.m_LogicalDevice);
            if (!Vulkan::Utils::IsResultSuccess(result))
            {
                softAssert(false, "VulkanRenderer::BeginFrame vkDeviceWaitIdle failed: '%s'", Vulkan::Utils::ResultToString(result));
                return false;
            }

            // If the swapchain recreation failed (because, for example, the window was minimized),
            // boot out before unsetting the flag.
            if (!RecreateSwapchain())
            {
                //softAssert(false, "Could not recreate swapchain!");
                return false;
            }

            LogInfo(LogChannel::Graphics, "Resized!");
            return false;
        }

        // Wait for the execution of the current frame to complete. The fence being free will allow this one to move on.   
        VulkanFence& fence = m_InFlightFences[m_CurrentFrame];
        if (!fence.Wait(m_VulkanDevice.m_LogicalDevice, UINT64_MAX))
        {
            softAssert(false, "In flight fence wait failure!");
            return false;
        }

        if (!m_SwapChain.AcquireNextImageIndex(m_VulkanDevice, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], m_InFlightFences[m_CurrentFrame].GetHandle(), &m_ImageIndex))
        {
            softAssert(false, "Could not get next image index!");
            return false;
        }

        VulkanCommandBuffer& commandBuffer = m_GraphicsCommandBuffers[m_ImageIndex];
        commandBuffer.ResetBuffer();
        commandBuffer.BeginBuffer(isSingleUse, isRenderpassContinue, isSimultaneousUse);

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = static_cast<smfloat32>(m_VulkanDevice.m_FrameBufferHeight);
        viewport.width = static_cast<smfloat32>(m_VulkanDevice.m_FrameBufferWidth);
        viewport.height = -static_cast<smfloat32>(m_VulkanDevice.m_FrameBufferHeight);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = m_VulkanDevice.m_FrameBufferWidth;
        scissor.extent.height = m_VulkanDevice.m_FrameBufferHeight;

        vkCmdSetViewport(commandBuffer.GetHandle(), 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer.GetHandle(), 0, 1, &scissor);


        smVec4 renderArea = smVec4{0.0f, 0.0f, static_cast<smfloat32>(m_VulkanDevice.m_FrameBufferWidth), static_cast<smfloat32>(m_VulkanDevice.m_FrameBufferHeight)};

        m_MainRenderPass.Begin(renderArea, commandBuffer, m_SwapChain.GetFrameBuffers()[m_ImageIndex].GetHandle());

        return true;
	}

    smbool VulkanRenderer::EndFrame(Time time)
    {
        constexpr smuint32 submitCount = 1;

        VulkanCommandBuffer& commandBuffer = m_GraphicsCommandBuffers[m_ImageIndex];

        m_MainRenderPass.End(commandBuffer);

        commandBuffer.EndBuffer();

        if (VulkanFence* fence = m_ImagesInFlight[m_ImageIndex])
        {
            if (fence->GetHandle() != VK_NULL_HANDLE)
            {
                fence->Wait(m_VulkanDevice.m_LogicalDevice, UINT64_MAX);
            }
        }

        VulkanFence& inFlightFence = m_InFlightFences[m_ImageIndex];
        m_ImagesInFlight[m_ImageIndex] = &inFlightFence;

        inFlightFence.Reset(m_VulkanDevice.m_LogicalDevice);

        // Submit the queue and wait for the operation to complete.
        // Begin queue submission
        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

        // Command buffer(s) to be executed.
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer.GetHandle();

        // The semaphore(s) to be signaled when the queue is complete.
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrame];

        // Wait semaphore ensures that the operation cannot begin until the image is available.
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_ImageAvailableSemaphores[m_CurrentFrame];

        // Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio.
        // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent colour attachment
        // writes from executing until the semaphore signals (i.e. one frame is presented at a time)
        VkPipelineStageFlags flags[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.pWaitDstStageMask = flags;

        VkResult result = vkQueueSubmit(m_VulkanDevice.m_GraphicsQueue, submitCount, &submitInfo, m_InFlightFences[m_CurrentFrame].GetHandle());

        if (result != VK_SUCCESS)
        {
            softAssert(false, "Could not submit to a queue! %s", Vulkan::Utils::ResultToString(result));
        }

        commandBuffer.Submit();

        m_SwapChain.Present(m_VulkanDevice, m_RenderFinishedSemaphores[m_CurrentFrame], m_ImageIndex);

        return true;
    }

    smbool VulkanRenderer::RecreateSwapchain()
    {
        // If already being recreated, do not try again.
        if (m_IsRecreatingSwapchain) 
        {
            softAssert(false, "Already recreating swapchaing!");
            return false;
        }
        // Detect if the window is too small to be drawn to
        if (m_VulkanDevice.m_FrameBufferWidth == 0 || m_VulkanDevice.m_FrameBufferHeight == 0) 
        {
            //softAssert(false, "Recreating swapchaing with window dimension 0!");
            return false;
        }
        m_IsRecreatingSwapchain = true;
        vkDeviceWaitIdle(m_VulkanDevice.m_LogicalDevice);
        smZero(m_ImagesInFlight.data(), m_ImagesInFlight.size() * sizeof(VulkanFence));

        VulkanSwapChainArguments arguments;
        arguments.m_Device = &m_VulkanDevice.m_PhysicalDevice;
        arguments.m_Surface = &m_Surface;
        arguments.m_SurfaceCapabilities = &m_Capabilities;
        arguments.m_SurfaceFormats = &m_SurfaceFormats;
        arguments.m_PresentModes = &m_PresentModes;
        VulkanSwapChain::QuerySwapChainSupport(arguments);

        //TODO : Change this GetDeviceDepthFormat.
        //It looks like a getter function but it changes the swap chain!
        if (!m_SwapChain.GetDeviceDepthFormat())
        {
            GetVulkanDevice().m_DepthFormat = VK_FORMAT_UNDEFINED;
            softAssert(false, "Error getting device depth format!");
            return false;
        }

        m_SwapChain.Recreate(m_VulkanDevice.m_FrameBufferWidth, m_VulkanDevice.m_FrameBufferHeight);
        m_FrameBuffferLastGeneration = m_FrameBuffferGeneration;

        for (VulkanCommandBuffer& commandBuffer : m_GraphicsCommandBuffers)
        {
            commandBuffer.FreeBuffer(m_VulkanDevice.m_LogicalDevice, m_VulkanDevice.m_GraphicsCommandPool);
        }

        for (VulkanFramebuffer& framebuffer : m_SwapChain.GetFrameBuffers())
        {
            framebuffer.Shutdown({m_VulkanDevice.m_LogicalDevice, m_Allocator});
        }

        // Regenerate framebuffers
        m_SwapChain.RegenerateFramebuffers();

        CreateCommandBuffers();

        m_IsRecreatingSwapchain = false;

        return true;
    }

    void VulkanRenderer::CreateBuffers()
    {
        VkMemoryPropertyFlagBits memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        const smuint64 vertexBufferSize = sizeof(smVert3D) * 1024 * 1024;
        m_VertexBuffer.Create(vertexBufferSize, (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT), memoryPropertyFlags,true);
        m_GeometryVertexOffset = 0;

        const smuint64 indexBufferSize = sizeof(smuint32) * 1024 * 1024;
        m_IndexBuffer.Create(indexBufferSize, (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT), memoryPropertyFlags, true);
        m_GeometryIndexOffset = 0;

        LogInfo(LogChannel::Graphics, "Created Vulkan Buffers!");
    }

    void VulkanRenderer::DestroyBuffers()
    {
        m_IndexBuffer.Destroy();
        m_VertexBuffer.Destroy();
    }

    void VulkanRenderer::UploadData(VkCommandPool pool, VkFence fence, VkQueue queue,const VulkanBuffer& buffer, smuint64 offset, smuint64 size, const void* data)
    {
        // Create a host-visible staging buffer to upload to. Mark it as the source of the transfer.
        VkBufferUsageFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        VulkanBuffer staging;
        staging.Create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, flags, true);
        staging.LoadData(0, size, 0, data);

        // Perform the copy from staging to the device local buffer.
        staging.CopyTo(pool, fence, queue, buffer.GetHandle(), 0, offset, size);

        // Clean up the staging buffer.
        staging.Destroy();
    }

    void VulkanRenderer::UpdateGlobalState(smMat4 projection, smMat4 view, smVec3 viewPosition, smVec4 ambientColor, smint32 mode)
    {
        VulkanCommandBuffer* commandBuffer = &g_VulkanRenderer->GetGraphicsCommandBuffers()[g_VulkanRenderer->GetImageIndex()];

        m_ObjectShader.Use(commandBuffer);

        m_ObjectShader.GetGlobalUniformObject().m_Projection = projection;
        m_ObjectShader.GetGlobalUniformObject().m_View = view;

        // TODO: other ubo properties

        m_ObjectShader.UpdateGlobalState();
    }

    void VulkanRenderer::DrawGeometry(const GeometryData& data)
    {
        if (data.m_Geometry != nullptr && data.m_Geometry->m_Id == SM_INVALID_ID) 
        {
            return;
        }

        Vulkan::Types::GeometryData& geometryData = m_GeometryData[data.m_Geometry->m_Handle];
        VulkanCommandBuffer& commandBuffer = m_GraphicsCommandBuffers[m_ImageIndex];

        m_ObjectShader.Use(&commandBuffer); 
        m_ObjectShader.UpdateModel(data.m_Model);

        
        if (const Material* material = data.m_Geometry->m_Material)
        {
            m_ObjectShader.UpdateMaterial(*material);
        }
        else
        {
            m_ObjectShader.UpdateMaterial(smMaterialSystem().GetDefaultMaterial());
        }
        

        // Bind vertex buffer at offset.
        VkDeviceSize offsets[1] = { geometryData.m_VertexBufferOffset };
        vkCmdBindVertexBuffers(commandBuffer.GetHandle(), 0, 1, &m_VertexBuffer.GetHandle(), offsets);

        // Draw indexed or non-indexed.
        if (geometryData.m_IndexCount > 0) 
        {
            // Bind index buffer at offset.
            vkCmdBindIndexBuffer(commandBuffer.GetHandle(), m_IndexBuffer.GetHandle(), geometryData.m_IndexBufferOffset, VK_INDEX_TYPE_UINT32);

            // Issue the draw.
            vkCmdDrawIndexed(commandBuffer.GetHandle(), geometryData.m_IndexCount, 1, 0, 0, 0);
        }
        else 
        {
            vkCmdDraw(commandBuffer.GetHandle(), geometryData.m_VertexCount, 1, 0, 0);
        }
    }

#pragma region Texture
    smbool VulkanRenderer::CreateTexture(const smuint8* pixels, Texture* texture)
    {
        // TODO: [GRAPHICS][ALLOCATION] use allocators
        texture->m_Data = gpAllocTexture(sizeof(VulkanTextureData));

        VulkanTextureData* data = static_cast<VulkanTextureData*>(texture->m_Data);

        VkDeviceSize imageSize = texture->m_Width * texture->m_Height * texture->m_ChannelCount;

        // WE USE 8 BITS PER CHANNEL
        VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;

        // Staging buffer with loaded data
        VkBufferUsageFlagBits usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        VulkanBuffer stagingBuffer;
        stagingBuffer.Create(imageSize, usage, memoryPropertyFlags, true);

        stagingBuffer.LoadData(0, imageSize, 0, const_cast<smuint8*>(pixels));

        // NOTE: Lots of assumptions here, different texture types will require
        // different options here.

        constexpr smbool shouldCreateView = true;

        VulkanImage* image = &data->m_Image;

        VulkanImage::CreateImage
        (
			g_VulkanRenderer->GetVulkanDevice().m_PhysicalDevice,
			g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice,
            g_VulkanRenderer->GetAllocator(),
            //TODO : [GRAPHICS][TEXTURES] make this configurable so we can use 3D textures
            VK_IMAGE_TYPE_2D,
            texture->m_Width,
            texture->m_Height,
            imageFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            shouldCreateView,
            VK_IMAGE_ASPECT_COLOR_BIT,
            image
        );

        if (image == nullptr)
        {
            softAssert(false, "Could not create image!");
            return false;;
        }

        VulkanCommandBuffer buffer;
        VkCommandPool graphicsCommandPool = g_VulkanRenderer->GetVulkanDevice().m_GraphicsCommandPool;
        VkQueue graphicsQueue = g_VulkanRenderer->GetVulkanDevice().m_GraphicsQueue;

        buffer.BeginSingleUseBuffer(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, graphicsCommandPool);

        //Transition layouts to receive data
        image->TransitionLayout(&buffer, imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        // Copy the data from the buffer.
        image->CopyImageFromBuffer(&buffer, stagingBuffer.GetHandle());
        //optimal data receiving -> shader read-only optimal layout.
        image->TransitionLayout(&buffer, imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        buffer.EndSingleUseBuffer(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, graphicsCommandPool, graphicsQueue);

        //Sampler
        VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        // TODO : [GRAPHICS][TEXTURE] Not configurable
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = 16;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        VkResult result = vkCreateSampler(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, &samplerInfo, g_VulkanRenderer->GetAllocator(), &data->m_Sampler);
        if (!Vulkan::Utils::IsResultSuccess(VK_SUCCESS))
        {
            softAssert(false, "Error creating texture sampler. Error : %s", Vulkan::Utils::ResultToString(result));
            return false;
        }

        return true;
    }

    void VulkanRenderer::DestroyTexture(Texture* texture)
    {
        if (texture == nullptr)
        {
            softAssert(false, "[VulkanRenderer]: Texture is null!");
            return;
        }

        VulkanTextureData* data = static_cast<VulkanTextureData*>(texture->m_Data);
        VulkanImage& image = data->m_Image;

        VulkanImage::DeleteImage(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, g_VulkanRenderer->GetAllocator(), image);

        vkDestroySampler(g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice, data->m_Sampler, g_VulkanRenderer->GetAllocator());
        data->m_Sampler = 0;

        gpFreeTexture(data, sizeof(VulkanTextureData));
        smZero(texture, sizeof(Texture));
    }
#pragma endregion

#pragma region Material
    smbool VulkanRenderer::CreateMaterial(Material* material)
    {
        if (material == nullptr)
        {
            softAssert(false, "Trying to create material on nullptr!");
            return false;
        }

        if (m_ObjectShader.AcquireObjectShaderResources(material) == SM_INVALID_ID)
        {
            softAssert(false, "Failed to acquire material shader resources!");
            return false;
        }

        return true;
    }

    void VulkanRenderer::DestroyMaterial(Material* material)
    {
        if (material == nullptr || material->m_ID == SM_INVALID_ID)
        {
            softAssert(false, "Invalid material (nullptr or invalid id)");
            return;
        }

        m_ObjectShader.ReleaseObjectShaderResources(material);
    }
#pragma endregion

#pragma region Geometry
    smbool VulkanRenderer::CreateGeometry(Geometry* geometry, smuint32 vertexCount, const smVert3D* vertices, smuint32 indexCount, const smuint32* indices)
    {
        if (vertexCount == 0 || vertices == nullptr || geometry == nullptr) 
        {
            LogError(LogChannel::Graphics, "CreateGeometry called with 0 vertex count or nullptr vertices or geometry nullptr");
            return false;
        }

        smbool isNewUpload = geometry->m_Handle == SM_INVALID_ID;
        Vulkan::Types::GeometryData oldData;

        Vulkan::Types::GeometryData* data = nullptr;
        
        if (isNewUpload)
        {
            smuint32 newId = 0;
            for (Vulkan::Types::GeometryData& d : m_GeometryData)
            {
                if (d.m_Id == SM_INVALID_ID)
                {
                    geometry->m_Handle = newId;
                    data = &d;
                    data->m_Id = newId;
                    break;
                }
                ++newId;
            }
        }
        else 
        {
            
            data = &m_GeometryData[geometry->m_Handle];
            oldData = *data;
        }

        hardAssert(data != nullptr, "Could not find free geometry data slot!");

        VkCommandPool pool = m_VulkanDevice.m_GraphicsCommandPool;
        VkQueue queue = m_VulkanDevice.m_GraphicsQueue;

        data->m_VertexBufferOffset = m_GeometryVertexOffset;
        data->m_VertexCount = vertexCount;
        data->m_VertexSize = sizeof(smVert3D) * vertexCount;

        UploadData(pool, 0, queue, m_VertexBuffer, data->m_VertexBufferOffset, data->m_VertexSize, vertices);
        //TODO : [GRAPHICS][GEOMETRY] This is a hack, we should use freelist
        m_GeometryVertexOffset += data->m_VertexSize;

        if (indexCount != 0 && indices != nullptr)
        {
            data->m_IndexBufferOffset = m_GeometryIndexOffset;
            data->m_IndexCount = indexCount;
            data->m_IndexSize = sizeof(smuint32) * indexCount;
            UploadData(pool, 0, queue, m_IndexBuffer, data->m_IndexBufferOffset, data->m_IndexSize, indices);

            m_GeometryIndexOffset += data->m_IndexSize;
            //TODO : [GRAPHICS][GEOMETRY] This is a hack, we should use freelist
        }

        if (data->m_Generation == SM_INVALID_ID) 
        {
            data->m_Generation = 0;
        }
        else 
        {
            data->m_Generation++;
        }

        if (!isNewUpload) 
        {
            m_VertexBuffer.Free(oldData.m_VertexBufferOffset, oldData.m_VertexSize);
            if (oldData.m_IndexSize > 0) 
            {
                m_IndexBuffer.Free(oldData.m_IndexBufferOffset, oldData.m_IndexSize);
            }
        }

        return true;
    }

    void VulkanRenderer::DestroyGeometry(Geometry* geometry)
    {
        if (geometry != nullptr && geometry->m_Handle != SM_INVALID_ID) 
        {
            vkDeviceWaitIdle(m_VulkanDevice.m_LogicalDevice);
            Vulkan::Types::GeometryData& geometryData = m_GeometryData[geometry->m_Handle];

            m_VertexBuffer.Free(geometryData.m_VertexBufferOffset, geometryData.m_VertexSize);

            if (geometryData.m_IndexSize > 0)
            {
                m_IndexBuffer.Free(geometryData.m_IndexBufferOffset, geometryData.m_IndexSize);
            }

            geometryData.m_Id = geometryData.m_Generation = SM_INVALID_ID;
        }
    }
#pragma endregion

#pragma region ObjectShader
    smbool VulkanRenderer::CreateObjectShader(Shader* shader)
    {
        softAssert(false, "Not implemented!");
        return false;
    }

    void VulkanRenderer::DestroyObjectShader(Shader* shader)
    {
        softAssert(false, "Not implemented!");
    }

    smbool VulkanRenderer::UseObjectShader(Shader* shader)
    {
        return smbool();
    }

    smbool VulkanRenderer::ObjectShaderBindGlobals(Shader* shader)
    {
        return smbool();
    }

    smbool VulkanRenderer::ObjectShaderBindInstances(Shader* shader)
    {
        return smbool();
    }

    smbool VulkanRenderer::ObjectShaderSetUniform(Shader* shader, const ShaderUniform& uniform, void* value)
    {
        return smbool();
    }

    smbool VulkanRenderer::ObjectShaderApplyGlobals(Shader* shader)
    {
        return smbool();
    }

    smbool VulkanRenderer::ObjectShaderApplyInstances(Shader* shader)
    {
        return smbool();
    }

    smbool VulkanRenderer::ObjectShaderBindInstance(Shader* shader, smuint32 instanceId)
    {
        return smbool();
    }
#pragma endregion

}

END_NAMESPACE