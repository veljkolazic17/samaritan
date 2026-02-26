#include <defines.hpp>
#if IMGUI_DISPLAY_ENABLED

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_vulkan.h"

#include <imgui/imguidrawmodule.hpp>

#include <engine/Engine.hpp>
#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>

BEGIN_NAMESPACE

static VkDescriptorPool s_ImGuiPool = VK_NULL_HANDLE;

void ImguiDrawModule::InitWindow_PlatformSpecific()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

#ifdef TARGET_WIN
#ifdef WINDOW_NATIVE_WIN
    const Graphics::Window* window = Engine::Engine::GetInstance().GetWindow();
    HWND hwnd = window->GetWindowState()->m_Hwnd;
    ImGui_ImplWin32_Init(hwnd);
#endif
#endif
}

void ImguiDrawModule::InitRenderer_PlatformSpecific()
{
    Graphics::VulkanDevice& device = Graphics::g_VulkanRenderer->GetVulkanDevice();

    VkDescriptorPoolSize poolSize = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;

    VulkanCheckResult(
        vkCreateDescriptorPool(device.m_LogicalDevice, &poolInfo, Graphics::g_VulkanRenderer->GetAllocator(), &s_ImGuiPool),
        "Failed to create ImGui descriptor pool!"
    );

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance       = Graphics::g_VulkanRenderer->GetInstance();
    initInfo.PhysicalDevice = device.m_PhysicalDevice;
    initInfo.Device         = device.m_LogicalDevice;
    initInfo.QueueFamily    = device.m_QueuesInfo.m_GraphicsIndex;
    initInfo.Queue          = device.m_GraphicsQueue;
    initInfo.DescriptorPool = s_ImGuiPool;
    initInfo.MinImageCount  = SM_MAX_FRAMES_IN_FLIGHT;
    initInfo.ImageCount     = Graphics::g_VulkanRenderer->GetVulkanSwapChain().GetImageCount();
    initInfo.MSAASamples    = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&initInfo, Graphics::g_VulkanRenderer->GetRenderpass("world").GetHandle());

    ImGui_ImplVulkan_CreateFontsTexture();
}

void ImguiDrawModule::NewFrame_PlatformSpecific()
{
    ImGui_ImplVulkan_NewFrame();
#ifdef TARGET_WIN
#ifdef WINDOW_NATIVE_WIN
    ImGui_ImplWin32_NewFrame();
#endif
#endif
    ImGui::NewFrame();
}

void ImguiDrawModule::Render_PlatformSpecific()
{
    DrawImgui();

    ImGui::Render();

    Graphics::VulkanCommandBuffer& cmdBuffer =
        Graphics::g_VulkanRenderer->GetGraphicsCommandBuffers()[Graphics::g_VulkanRenderer->GetImageIndex()];

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer.GetHandle());
}

void ImguiDrawModule::Shutdown_PlatformSpecific()
{
    vkDeviceWaitIdle(Graphics::g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice);

    ImGui_ImplVulkan_Shutdown();
#ifdef TARGET_WIN
#ifdef WINDOW_NATIVE_WIN
    ImGui_ImplWin32_Shutdown();
#endif
#endif
    ImGui::DestroyContext();

    vkDestroyDescriptorPool(
        Graphics::g_VulkanRenderer->GetVulkanDevice().m_LogicalDevice,
        s_ImGuiPool,
        Graphics::g_VulkanRenderer->GetAllocator()
    );
    s_ImGuiPool = VK_NULL_HANDLE;
}

END_NAMESPACE

#endif
