#pragma once
#include <defines.hpp>
#include <objecttemplates/shutdownable.hpp>

#ifdef TARGET_WIN
#ifdef WINDOW_NATIVE_WIN
#include <windows.h>
#include <windowsx.h>
#endif
#endif

BEGIN_NAMESPACE

namespace Graphics
{
	struct WindowState
	{
#ifdef TARGET_WIN
#ifdef WINDOW_NATIVE_WIN
		HINSTANCE m_Instance;
		HWND m_Hwnd;
#endif
#endif
	};

	class Window : public Shutdownable
	{
	public:
		Window(muint64 width, muint64 height, muint64 x, muint64 y, mcstring windowName)
			: m_Width(width),
			m_Height(height),
			m_X(x),
			m_Y(y),
			m_WindowName(windowName){}
		Window(){}

		SM_INLINE muint64 GetWidth() { return m_Width; }
		SM_INLINE muint64 GetHeight() { return m_Height; }

		SM_INLINE void SetWidth(muint64 width) { m_Width = width; }
		SM_INLINE void SetHeight(muint64 height) { m_Height = height; }

		void Init() override;
		void Shutdown() override;

		SM_INLINE const WindowState* const GetWindowState() const { return m_WindowState; }

	private:
		muint64 m_Width = 0;
		muint64 m_Height = 0;
		muint64 m_X = 0;
		muint64 m_Y = 0;
		mcstring m_WindowName = nullptr;
		WindowState* m_WindowState = nullptr;
	};
}

END_NAMESPACE