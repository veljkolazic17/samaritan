#ifdef WINDOW_GLFW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <engine/graphics/window.hpp>
#include <utils/asserts/assert.hpp>
#include <engine/memory/memory.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
    struct WindowState
    {
        GLFWwindow* m_GLFWwindow;
    };

    void Window::Init()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_WindowState = (WindowState*)Memory::mmaloc(sizeof(WindowState), false);
        if (m_WindowState == nullptr)
        {
            hardAssert(false, "Cannot allocate window state!");
        }

        m_WindowState->m_GLFWwindow = glfwCreateWindow(m_Width, m_Height, m_WindowName, nullptr, nullptr);
    }

    void Window::Shutdown()
    {
        if (GLFWwindow* window = m_WindowState->m_GLFWwindow)
        {
            glfwDestroyWindow(window);
        }
        Memory::mmfree(m_WindowState, false);
        glfwTerminate();
    }
}

END_NAMESPACE

#endif