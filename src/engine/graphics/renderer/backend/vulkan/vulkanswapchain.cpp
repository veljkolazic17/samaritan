#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanswapchain.h>

#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>

#include <math/math.hpp>
#include <utils/asserts/assert.hpp>
#include <utils/logger/log.hpp>

BEGIN_NAMESPACE

namespace Graphics
{

	void VulkanSwapChain::QuerySwapChainSupport(VulkanSwapChainArguments& queryArguments)
	{
		// ???? not sure if this should be done like this
		hardAssert(queryArguments.IsVaild(), "Swap chain arguments not initialized");
		//Check swap chain support
		muint32 formatCount = 0;
		muint32 presentModeCount = 0;

		// Surface capabilities
		VulkanCheckResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*queryArguments.m_Device, *queryArguments.m_Surface, queryArguments.m_SurfaceCapabilities), "Error getting Surface Capabilities!");

		// Surface formats
		VulkanCheckResult(vkGetPhysicalDeviceSurfaceFormatsKHR(*queryArguments.m_Device, *queryArguments.m_Surface, &formatCount, 0), "Error getting format count");

		if (formatCount != 0)
		{
			VulkanCheckResult(vkGetPhysicalDeviceSurfaceFormatsKHR(*queryArguments.m_Device, *queryArguments.m_Surface, &formatCount, queryArguments.m_SurfaceFormats->data()), "Error getting formats");
		}
		else
		{
			hardAssert(false, "Required swapchain support not present!");
		}

		// Present modes
		VulkanCheckResult(vkGetPhysicalDeviceSurfacePresentModesKHR(*queryArguments.m_Device, *queryArguments.m_Surface, &presentModeCount, 0), "Error getting present count!");

		if (presentModeCount != 0)
		{
			VulkanCheckResult(vkGetPhysicalDeviceSurfacePresentModesKHR(*queryArguments.m_Device, *queryArguments.m_Surface, &presentModeCount, queryArguments.m_PresentModes->data()), "Error getting present modes!");
		}
		else
		{
			hardAssert(false, "Error getting present modes!");
		}
	}

	void VulkanSwapChain::Create(muint32 width, muint32 height)
	{
		//CreateInternal(width, height);
	}

	void VulkanSwapChain::Recreate(muint32 width, muint32 height)
	{
		DestroyInternal();
		//CreateInternal(width, height);
	}

	void VulkanSwapChain::Destroy()
	{
		DestroyInternal();
	}


	mbool VulkanSwapChain::AcquireNextImageIndex
	(
		VulkanDevice& device,
		muint64 timeout, 
		VkSemaphore& semaphore, 
		VkFence& fence,
		muint32* outImageIndex
	)
	{
		VkResult result  = vkAcquireNextImageKHR(device.m_LogicalDevice, m_Handle, timeout, semaphore, fence, outImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			Recreate(device.m_FrameBufferWidth, device.m_FrameBufferHeight);
			return false;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			hardAssert(false, "Error acquiring new image index!");
			return false;
		}
		return true;
	}

	void VulkanSwapChain::Present(VulkanDevice& device, VkSemaphore& renderCompletionSemaphore, muint32 imageToPresnet)
	{
		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderCompletionSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Handle;
		presentInfo.pImageIndices = &imageToPresnet;
		presentInfo.pResults = 0;

		VkResult result = vkQueuePresentKHR(device.m_PresentQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
		{
			// Swapchain is out of date, suboptimal or a framebuffer resize has occurred. Trigger swapchain recreation.
			Recreate(device.m_FrameBufferWidth, device.m_FrameBufferHeight);
		}
		else if (result != VK_SUCCESS) 
		{
			hardAssert(false, "Failed to present swap chain image!");
		}
	}


	// This fucking queryArguments is passed from vulkanbackend and it is set of beckend fields
	void VulkanSwapChain::CreateInternal(muint32 width, muint32 height)
	{
		hardAssert(m_Renderer != nullptr, "Renderer not set for surface!");

		VkExtent2D swapchainExtent = { width, height };
		m_MaxFrames = 2;

		// Choose a swap surface format.
		mbool found = false;
		for (const VkSurfaceFormatKHR& supportedFormat : m_Renderer->GetSurfaceFormats())
		{
			// Preferred formats
			if (supportedFormat.format == VK_FORMAT_B8G8R8A8_UNORM && supportedFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				m_ImageFormat = supportedFormat;
				found = true;
				break;
			}
		}
		if (!found) 
		{
			m_ImageFormat =  (m_Renderer->GetSurfaceFormats())[0];
		}

		found = false;
		for (const VkPresentModeKHR& presnetMode : m_Renderer->GetPresentModes())
		{
			if (presnetMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				m_PresentMode = presnetMode;
				found = true;
				break;
			}
		}
		if (!found)
		{
			m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
		}

		//Query SwapChain Support
		VulkanSwapChainArguments queryArguments;
		queryArguments.m_Device = &(m_Renderer->GetVulkanDevice().m_PhysicalDevice);
		queryArguments.m_Surface = &(m_Renderer->GetSurface());
		queryArguments.m_SurfaceCapabilities = &(m_Renderer->GetCapabilities());
		queryArguments.m_SurfaceFormats = &(m_Renderer->GetSurfaceFormats());
		queryArguments.m_PresentModes = &(m_Renderer->GetPresentModes());
		VulkanSwapChain::QuerySwapChainSupport(queryArguments);

		// Swapchain extent
		VkSurfaceCapabilitiesKHR* surfaceCapabilities = queryArguments.m_SurfaceCapabilities;

		//Is this needed?
		if (surfaceCapabilities != nullptr)
		{
			if (surfaceCapabilities->currentExtent.width != UINT32_MAX)
			{
				swapchainExtent = surfaceCapabilities->currentExtent;
			}
		}
		
		// Clamp to the value allowed by the GPU.
		VkExtent2D min = surfaceCapabilities->minImageExtent;
		VkExtent2D max = surfaceCapabilities->maxImageExtent;

		swapchainExtent.width = Math::ClampInt(swapchainExtent.width, min.width, max.width);
		swapchainExtent.height = Math::ClampInt(swapchainExtent.height, min.height, max.height);

		muint32 image_count = surfaceCapabilities->minImageCount + 1;
		if (surfaceCapabilities->maxImageCount > 0 && image_count > surfaceCapabilities->maxImageCount)
		{
			image_count = surfaceCapabilities->maxImageCount;
		}

		const VkPhysicalDevice* vpd = queryArguments.m_Device;
		const VkSurfaceKHR* surface = queryArguments.m_Surface;

		// Swapchain create info
		VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		swapchainCreateInfo.surface = *surface;
		swapchainCreateInfo.minImageCount = image_count;
		swapchainCreateInfo.imageFormat = m_ImageFormat.format;
		swapchainCreateInfo.imageColorSpace = m_ImageFormat.colorSpace;
		swapchainCreateInfo.imageExtent = swapchainExtent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//TODO : Too much same stuff
		muint32 graphicsQueueIndex = m_Renderer->GetVulkanDevice().m_QueuesInfo.m_GraphicsIndex;
		muint32 presentQueueIndex = m_Renderer->GetVulkanDevice().m_QueuesInfo.m_PresentIndex;

		// Setup the queue family indices
		if (graphicsQueueIndex != presentQueueIndex)
		{
			muint32 queueFamilyIndices[2] = 
			{
				graphicsQueueIndex,
				presentQueueIndex
			};

			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else 
		{
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCreateInfo.queueFamilyIndexCount = 0;
			swapchainCreateInfo.pQueueFamilyIndices = 0;
		}

		swapchainCreateInfo.preTransform = m_Renderer->GetCapabilities().currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = m_PresentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = 0;

		VulkanCheckResult(vkCreateSwapchainKHR(m_Renderer->GetVulkanDevice().m_LogicalDevice, &swapchainCreateInfo, m_Renderer->GetAllocator(), &m_Handle), "Error creating Swapchain!");

		m_Renderer->SetCurrentFrame(0);

		// Images
		muint32 imageCount = 0;
		VulkanCheckResult(vkGetSwapchainImagesKHR(m_Renderer->GetVulkanDevice().m_LogicalDevice, m_Handle, &imageCount, 0), "Error getting swapchain image count!");
		VulkanCheckResult(vkGetSwapchainImagesKHR(m_Renderer->GetVulkanDevice().m_LogicalDevice, m_Handle, &imageCount, m_Images.data()), "Error creating swapchain images!");

		// Views
		for (const VkImage& image : m_Images) 
		{
			VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = m_ImageFormat.format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			VkImageView imageView;
			VulkanCheckResult(vkCreateImageView(m_Renderer->GetVulkanDevice().m_LogicalDevice, &viewInfo, m_Renderer->GetAllocator(), &imageView), "Error creating image view!");
			m_ImageViews.emplace_back(imageView);
		}

		//TODO : Create depth buffer (needed only for 3D!)

		LogInfo(LogChannel::Graphics, "Swapchain created!");
	}

	void VulkanSwapChain::DestroyInternal()
	{

	}
}

END_NAMESPACE
