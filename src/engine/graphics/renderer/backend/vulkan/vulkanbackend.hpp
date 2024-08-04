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

		void Resize(muint32 width, muint32 heigth) override;
		void BeginFrame(Time time) override;
		void EndFrame(Time time) override;

	private:
		void GetPlatformExtensions(std::vector<const char*>& platfromExtensions);
		void CreatePlatformSurface();
		void SelectVPD();
		void CreateDevice();

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
		VkSurfaceCapabilitiesKHR m_Capabilities;

#ifdef DEBUG
		VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif
	};
}

END_NAMESPACE