#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>

#include <engine/graphics/renderer/backend/vulkan/vulkandevice.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanimage.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanframebuffer.hpp>

class VulkanImage;

BEGIN_NAMESPACE

namespace Graphics
{
	class VulkanRenderer;

	struct VulkanSwapChainArguments
	{
		const VkPhysicalDevice* m_Device;
		const VkSurfaceKHR* m_Surface;
		VkSurfaceCapabilitiesKHR* m_SurfaceCapabilities;
		std::vector<VkSurfaceFormatKHR>* m_SurfaceFormats;
		std::vector<VkPresentModeKHR>* m_PresentModes;

		mbool IsVaild()
		{
			return m_Device != nullptr && m_Surface != nullptr && m_SurfaceCapabilities != nullptr && m_SurfaceFormats != nullptr && m_PresentModes != nullptr;
		}
	};

	class VulkanSwapChain
	{
	public:
		void Create(muint32 width, muint32 height);
		void Recreate(muint32 width, muint32 height);
		void Destroy();
		mbool AcquireNextImageIndex(VulkanDevice& device, muint64 timeout, VkSemaphore& semaphore, VkFence& fence, muint32* outImageIndex);
		void Present(VulkanDevice& device, VkSemaphore& renderCompletionSemaphore, muint32 imageToPresnet);
		void RegenerateFramebuffers();

		SM_INLINE VkSurfaceFormatKHR& GetImageFormat() { return m_ImageFormat; }
		SM_INLINE VkPresentModeKHR& GetPresentMode() { return m_PresentMode; }
		SM_INLINE VkSwapchainKHR& GetSwapChainHandle() { return m_Handle; }
		SM_INLINE std::vector<VkImage>& GetImages() { return m_Images; }
		SM_INLINE std::vector<VkImageView>& GetImageViews() { return m_ImageViews; }
		SM_INLINE std::vector<VulkanFramebuffer>& GetFrameBuffers() { return m_Framebuffers; }

		SM_INLINE void SetVulkanRenderer(VulkanRenderer* renderer) { m_Renderer = renderer; }
		static void QuerySwapChainSupport(VulkanSwapChainArguments& queryArguments);

		mbool GetDeviceDepthFormat();

	private:
		void CreateInternal(muint32 width, muint32 height);
		void DestroyInternal();

		VkSurfaceFormatKHR m_ImageFormat;
		VkPresentModeKHR m_PresentMode;
		muint8 m_MaxFrames = 0;
		VkSwapchainKHR m_Handle;
		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
		std::vector<VulkanFramebuffer> m_Framebuffers;

		//This is terrible
		VulkanRenderer* m_Renderer;
		//TODO : Check if this fucking makes sense
		VulkanImage m_DepthBufferImage;
	};
}

END_NAMESPACE