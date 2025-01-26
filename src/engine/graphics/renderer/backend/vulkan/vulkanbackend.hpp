#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.inl>
#include <engine/graphics/renderer/backend/rendererbackend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbuffer.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkandevice.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanswapchain.h>
#include <engine/graphics/renderer/backend/vulkan/vulkanrenderpass.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkancommandbuffer.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanfence.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanobjectshader.hpp>

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
		mbool BeginFrame(Time time) override;
		mbool EndFrame(Time time) override;
		void UpdateGlobalState(smMat4 projection, smMat4 view, smVec3 viewPosition, smVec4 ambientColor, mint32 mode) override;
		void UpdateObject(smMat4 model) override;

		SM_INLINE VkAllocationCallbacks* GetAllocator() { return m_Allocator; }
		SM_INLINE VulkanDevice& GetVulkanDevice() { return m_VulkanDevice; }
		SM_INLINE VkSurfaceKHR& GetSurface() { return m_Surface; }
		SM_INLINE std::vector<VkSurfaceFormatKHR>& GetSurfaceFormats() { return m_SurfaceFormats; }
		SM_INLINE std::vector<VkPresentModeKHR>& GetPresentModes() { return m_PresentModes; }
		SM_INLINE VkSurfaceCapabilitiesKHR& GetCapabilities() { return m_Capabilities; }
		SM_INLINE VulkanSwapChain& GetVulkanSwapChain() { return m_SwapChain; }
		SM_INLINE VulkanRenderpass& GetMainRenderpass() { return m_MainRenderPass; }
		SM_INLINE std::vector<VulkanCommandBuffer>& GetGraphicsCommandBuffers() { return m_GraphicsCommandBuffers; }
		SM_INLINE muint32 GetImageIndex() { return m_ImageIndex; }

		SM_INLINE muint64 GetCurrentFrame() { return m_CurrentFrame; }
		SM_INLINE void SetCurrentFrame(muint64 frame) { m_CurrentFrame = frame; }
	private:
		void GetPlatformExtensions(std::vector<const char*>& platfromExtensions);
		void CreatePlatformSurface();
		void SelectVPD();
		void CreateVLD();
		void CreateDevice();
		void DestroyDevice();
		void CreateCommandBuffers();
		void DestroyCommandBuffers();
		void CreateCommandPool();
		void DestroyCommandPool();
		void CreateSyncObjects();
		void DestroySyncObjects();
		mbool RecreateSwapchain();
		void CreateBuffers();
		void DestroyBuffers();
		void UploadData(VkCommandPool pool, VkFence fence, VkQueue queue, VulkanBuffer* buffer, muint64 offset, muint64 size, void* data);

		mbool CheckDeviceRequerments
		(
			const VkPhysicalDevice& device,
			const VkPhysicalDeviceProperties& properties,
			const VkPhysicalDeviceFeatures& features,
			const VkPhysicalDeviceMemoryProperties& memory
		);

		//TODO : Sort fields by size for better memory structure!
		std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
		std::vector<VkPresentModeKHR> m_PresentModes;

		std::vector<VulkanCommandBuffer> m_GraphicsCommandBuffers;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VulkanFence> m_InFlightFences;
		std::vector<VulkanFence*> m_ImagesInFlight;

		VulkanSwapChain m_SwapChain;
		VulkanRenderpass m_MainRenderPass;
		VkSurfaceCapabilitiesKHR m_Capabilities;

		VulkanObjectShader m_ObjectShader;
		VulkanBuffer m_IndexBuffer;
		VulkanBuffer m_VertexBuffer;

		VkInstance m_Instance;
		VkAllocationCallbacks* m_Allocator = nullptr;
		VulkanDevice m_VulkanDevice;
		VkSurfaceKHR m_Surface;

		muint64 m_CurrentFrame = 0;
		muint64 m_FrameBuffferGeneration = 0;
		muint64 m_FrameBuffferLastGeneration = 0;
		muint64 m_GeometryIndexOffset = 0;
		muint64 m_GeometryVertexOffset = 0;
		muint32 m_ImageIndex = 0;
		mbool m_IsRecreatingSwapchain = false;
#ifdef DEBUG
		VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif
	};

	extern VulkanRenderer* g_VulkanRenderer;
}

END_NAMESPACE