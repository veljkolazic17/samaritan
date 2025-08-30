#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.hpp>

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

		smbool IsVaild()
		{
			return m_Device != nullptr && m_Surface != nullptr && m_SurfaceCapabilities != nullptr && m_SurfaceFormats != nullptr && m_PresentModes != nullptr;
		}
	};

	class VulkanSwapChain
	{
	public:
		void Create(smuint32 width, smuint32 height);
		void Recreate(smuint32 width, smuint32 height);
		void Destroy();
		smbool AcquireNextImageIndex(VulkanDevice& device, smuint64 timeout, VkSemaphore& semaphore, VkFence& fence, smuint32* outImageIndex);
		void Present(VulkanDevice& device, VkSemaphore& renderCompletionSemaphore, smuint32 imageToPresnet);
		void RegenerateFramebuffers();

		SM_INLINE VkSurfaceFormatKHR& GetImageFormat() { return m_ImageFormat; }
		SM_INLINE VkPresentModeKHR& GetPresentMode() { return m_PresentMode; }
		SM_INLINE VkSwapchainKHR& GetSwapChainHandle() { return m_Handle; }
		SM_INLINE smuint8 GetImageCount() { return m_ImageCount; }
		SM_INLINE std::vector<VkImage>& GetImages() { return m_Images; }
		SM_INLINE std::vector<VkImageView>& GetImageViews() { return m_ImageViews; }
		SM_INLINE std::vector<VulkanFramebuffer>& GetFrameBuffers() { return m_Framebuffers; }

		static void QuerySwapChainSupport(VulkanSwapChainArguments& queryArguments);

		smbool GetDeviceDepthFormat();

	private:
		void CreateInternal(smuint32 width, smuint32 height);
		void DestroyInternal();

		VkSurfaceFormatKHR m_ImageFormat;
		VkPresentModeKHR m_PresentMode;
		VkSwapchainKHR m_Handle;
		smuint8 m_ImageCount = 0;
		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
		std::vector<VulkanFramebuffer> m_Framebuffers;

		VulkanImage m_DepthBufferImage;
	};
}

END_NAMESPACE