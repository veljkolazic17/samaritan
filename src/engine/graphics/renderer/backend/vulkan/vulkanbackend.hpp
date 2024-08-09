#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>
#include <engine/graphics/renderer/backend/rendererbackend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkandevice.hpp>
#include <vector>

BEGIN_NAMESPACE

namespace Graphics
{
	class VulkanRenderer : public RendererBackend
	{
	public:
		DEFINE_RENDERER(RendererType::Vulkan);

		void Init() override;
		void Shutdown() override;

		void Resize(muint32 width, muint32 height) override;
		void BeginFrame(Time time) override;
		void EndFrame(Time time) override;

		SM_INLINE VkAllocationCallbacks* GetAllocator() { return m_Allocator; }
		SM_INLINE VulkanDevice& GetVulkanDevice() { return m_VulkanDevice; }
		SM_INLINE VkSurfaceKHR& GetSurface() { return m_Surface; }
		SM_INLINE std::vector<VkSurfaceFormatKHR>& GetSurfaceFormats() { return m_SurfaceFormats; }
		SM_INLINE std::vector<VkPresentModeKHR>& GetPresentModes() { return m_PresentModes; }
		SM_INLINE VkSurfaceCapabilitiesKHR& GetCapabilities() { return m_Capabilities; }

		SM_INLINE muint64 GetCurrentFrame() { return m_CurrentFrame; }
		SM_INLINE void SetCurrentFrame(muint64 frame) { m_CurrentFrame = frame; }

	private:
		void GetPlatformExtensions(std::vector<const char*>& platfromExtensions);
		void CreatePlatformSurface();
		void SelectVPD();
		void CreateVLD();
		void CreateDevice();
		void DestroyDevice();

		mbool CheckDeviceRequerments
		(
			const VkPhysicalDevice& device,
			const VkPhysicalDeviceProperties& properties,
			const VkPhysicalDeviceFeatures& features,
			const VkPhysicalDeviceMemoryProperties& memory
		);

		VkInstance m_Instance;
		VkAllocationCallbacks* m_Allocator = nullptr;
		VulkanDevice m_VulkanDevice;
		VkSurfaceKHR m_Surface;

		muint64 m_CurrentFrame = 0;

		std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
		std::vector<VkPresentModeKHR> m_PresentModes;
		VkSurfaceCapabilitiesKHR m_Capabilities;

#ifdef DEBUG
		VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif
	};
}

END_NAMESPACE