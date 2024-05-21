#ifdef TARGET_WIN
#ifdef WINDOW_NATIVE_WIN

#include <windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#include <engine/graphics/renderer/backend/vulkan/vulkanbackend.hpp>
#include <engine/graphics/window.hpp>
#include <engine/Engine.hpp>


BEGIN_NAMESPACE

namespace Graphics
{
	void VulkanRenderer::GetPlatformExtensions(std::vector<const char*>& platfromExtensions)
	{
		platfromExtensions.emplace_back("VK_KHR_win32_surface");
	}

	void VulkanRenderer::CreatePlatformSurface()
	{
        VkWin32SurfaceCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        const Window* const window = Engine::Engine::GetInstance().GetWindow();
        
        if (window != nullptr)
        {
            if (const WindowState* const windowState = window->GetWindowState())
            {
                createInfo.hinstance = windowState->m_Instance;
                createInfo.hwnd = windowState->m_Hwnd;

                VulkanCheckResult(vkCreateWin32SurfaceKHR(m_Instance, &createInfo, m_Allocator, &m_Surface), "Vulkan Surface not created!")
            }
            else
            {
                hardAssert(false, "Window not initialized");
            }
        }
        else
        {
            hardAssert(false, "Window is not created!");
        }
        
	}
}

END_NAMESPACE
#endif
#endif