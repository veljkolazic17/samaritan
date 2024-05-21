#ifdef TARGET_WIN
#ifdef WINDOW_NATIVE_WIN
#include <engine/graphics/window.hpp>
#include <windows.h>
#include <windowsx.h>
#include <utils/asserts/assert.hpp>
#include <engine/memory/memory.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
    struct WindowState
    {
        HINSTANCE m_Instance;
        HWND m_Hwnd;
    };

    LRESULT CALLBACK messageCallback(HWND hwnd, muint32 msg, WPARAM w_param, LPARAM l_param)
    {
        switch (msg) 
        {
            case WM_ERASEBKGND:
                // Notify the OS that erasing will be handled by the application to prevent flicker.
                return 1;
            case WM_CLOSE:
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_SIZE: 
            {
            } 
            break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP: 
            {
            } 
            break;
            case WM_MOUSEMOVE: 
            {
            } 
            break;
            case WM_MOUSEWHEEL: 
            {
            } 
            break;
            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP: 
            {
            } 
            break;
        }
        return DefWindowProcA(hwnd, msg, w_param, l_param);
    }

	void Window::Init()
	{
        m_WindowState = (WindowState*)Memory::mmaloc(sizeof(WindowState), false);
        if (m_WindowState == nullptr)
        {
            hardAssert(false, "Cannot allocate window state!");
        }

        m_WindowState->m_Instance = GetModuleHandleA(0);

        HICON icon = LoadIcon(m_WindowState->m_Instance, IDI_APPLICATION);
        WNDCLASSA windowClass;
        memset(&windowClass, 0, sizeof(windowClass));
        // Enable double clicks
        windowClass.style = CS_DBLCLKS;
        windowClass.lpfnWndProc = messageCallback;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = m_WindowState->m_Instance;
        windowClass.hIcon = icon;
        windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        // Transparent
        windowClass.hbrBackground = nullptr;
        windowClass.lpszClassName = "samaritanWindowClass";

        if (RegisterClassA(&windowClass) == false)
        {
            softAssert(false, "Cannot register window class!");
        }

        muint64 window_x = m_X;
        muint64 window_y = m_Y;
        muint64 window_width = m_Width;
        muint64 window_height = m_Height;

        mflags window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
        mflags window_ex_style = WS_EX_APPWINDOW;

        window_style |= WS_MAXIMIZEBOX;
        window_style |= WS_MINIMIZEBOX;
        window_style |= WS_THICKFRAME;

        // Obtain the size of the border.
        RECT border_rect = { 0, 0, 0, 0 };
        AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

        // In this case, the border rectangle is negative.
        window_x += border_rect.left;
        window_y += border_rect.top;

        // Grow by the size of the OS border.
        window_width += border_rect.right - border_rect.left;
        window_height += border_rect.bottom - border_rect.top;

        HWND handle = CreateWindowExA(
            window_ex_style, windowClass.lpszClassName, m_WindowName,
            window_style, window_x, window_y, window_width, window_height,
            0, 0, m_WindowState->m_Instance, 0);

        if (handle == nullptr) 
        {
            hardAssert(false, "Cannot create window!");
        }

        m_WindowState->m_Hwnd = handle;
        
        ShowWindow(m_WindowState->m_Hwnd, SW_SHOW);
	}

	void Window::Shutdown()
	{
        if (const HWND& hwnd = m_WindowState->m_Hwnd)
        {
            DestroyWindow(hwnd);
        }
        Memory::mmfree(m_WindowState, false);
	}
}

END_NAMESPACE

#endif
#endif