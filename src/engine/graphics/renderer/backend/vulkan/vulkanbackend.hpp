#pragma once
#include <defines.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkantypes.hpp>
#include <engine/graphics/renderer/backend/rendererbackend.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbuffer.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkandevice.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanswapchain.h>
#include <engine/graphics/renderer/backend/vulkan/vulkanrenderpass.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkancommandbuffer.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanfence.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanobjectshader.hpp>

#include <engine/resources/graphics/texture.hpp>
#include <vector>

BEGIN_NAMESPACE

//TODO : [GRAPHICS][VULKAN] Make this shit go away
#define SM_VULKAN_MAX_GEOMETRY_COUNT	4096

namespace Graphics
{
    class VulkanRenderer : public RendererBackend
    {
    public:
        DEFINE_RENDERER(RendererType::Vulkan);

        void Init() override;
        void Shutdown() override;

        void Resize(smuint32 width, smuint32 height) override;
        smbool BeginFrame(Time time) override;
        smbool EndFrame(Time time) override;
        void UpdateGlobalState(smMat4 projection, smMat4 view, smVec3 viewPosition, smVec4 ambientColor, smint32 mode) override;
        void DrawGeometry(const GeometryData& data) override;

        SM_INLINE VkAllocationCallbacks* GetAllocator() { return m_Allocator; }
        SM_INLINE VkInstance GetInstance() { return m_Instance; }
        SM_INLINE VulkanDevice& GetVulkanDevice() { return m_VulkanDevice; }
        SM_INLINE VkSurfaceKHR& GetSurface() { return m_Surface; }
        SM_INLINE std::vector<VkSurfaceFormatKHR>& GetSurfaceFormats() { return m_SurfaceFormats; }
        SM_INLINE std::vector<VkPresentModeKHR>& GetPresentModes() { return m_PresentModes; }
        SM_INLINE VkSurfaceCapabilitiesKHR& GetCapabilities() { return m_Capabilities; }
        SM_INLINE VulkanSwapChain& GetVulkanSwapChain() { return m_SwapChain; }
        SM_INLINE VulkanRenderpass& GetRenderpass(const smuint32 renderpassId) { return m_RenderPasses[renderpassId]; }
        SM_INLINE std::vector<VulkanCommandBuffer>& GetGraphicsCommandBuffers() { return m_GraphicsCommandBuffers; }
        SM_INLINE smuint32 GetImageIndex() { return m_ImageIndex; }

        SM_INLINE smuint64 GetCurrentFrame() { return m_CurrentFrame; }
        SM_INLINE void SetCurrentFrame(smuint64 frame) { m_CurrentFrame = frame; }

        smbool CreateTexture(const smuint8* pixels, Texture* texture) override;
        void DestroyTexture(Texture* texture) override;
        smbool CreateMaterial(Material* material) override;
        void DestroyMaterial(Material* material) override;
        smbool CreateGeometry(Geometry* geometry, smuint32 vertexCount, const smVert3D* vertices, smuint32 indexCount, const smuint32* indices) override;
        void DestroyGeometry(Geometry* geometry) override;
        
        //ObjectShader
        smbool CreateObjectShader(Shader* shader) override;
        void DestroyObjectShader(Shader* shader) override;
        smbool UseObjectShader(Shader* shader) override;
        smbool ObjectShaderBindGlobals(Shader* shader) override;
        smbool ObjectShaderBindInstances(Shader* shader) override;
        smbool ObjectShaderSetUniform(Shader* shader, const ShaderUniform& uniform, void* value) override;
        smbool ObjectShaderApplyGlobals(Shader* shader) override;
        smbool ObjectShaderApplyInstances(Shader* shader) override;
        smbool ObjectShaderBindInstance(Shader* shader, smuint32 instanceId) override;

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
        smbool RecreateSwapchain();
        void CreateBuffers();
        void DestroyBuffers();
        void UploadData(VkCommandPool pool, VkFence fence, VkQueue queue, const VulkanBuffer& buffer, smuint64 offset, smuint64 size, const void* data);

        smbool CheckDeviceRequerments
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
        std::unordered_map<smstring, VulkanRenderpass> m_RenderPasses;

        VulkanSwapChain m_SwapChain;
        VkSurfaceCapabilitiesKHR m_Capabilities;

        VulkanObjectShader m_ObjectShader;
        VulkanBuffer m_IndexBuffer;
        VulkanBuffer m_VertexBuffer;
        Vulkan::Types::GeometryData m_GeometryData[SM_VULKAN_MAX_GEOMETRY_COUNT];

        VkInstance m_Instance;
        VkAllocationCallbacks* m_Allocator = nullptr;
        VulkanDevice m_VulkanDevice;
        VkSurfaceKHR m_Surface;

        smuint64 m_CurrentFrame = 0;
        smuint64 m_FrameBuffferGeneration = 0;
        smuint64 m_FrameBuffferLastGeneration = 0;
        smuint64 m_GeometryIndexOffset = 0;
        smuint64 m_GeometryVertexOffset = 0;
        smuint32 m_ImageIndex = 0;
        smbool m_IsRecreatingSwapchain = false;
#ifdef DEBUG
        VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif
    };

    extern VulkanRenderer* g_VulkanRenderer;
}

END_NAMESPACE