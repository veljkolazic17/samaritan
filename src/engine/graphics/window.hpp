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
		Window(smuint64 width, smuint64 height, smuint64 x, smuint64 y, smcstring windowName)
			: m_Width(width),
			m_Height(height),
			m_X(x),
			m_Y(y),
			m_WindowName(windowName){}
		Window(){}

		SM_INLINE smuint64 GetWidth() { return m_Width; }
		SM_INLINE smuint64 GetHeight() { return m_Height; }

		SM_INLINE void SetWidth(smuint64 width) { m_Width = width; }
		SM_INLINE void SetHeight(smuint64 height) { m_Height = height; }

		void Init() override;
		void Shutdown() override;

		SM_INLINE const WindowState* const GetWindowState() const { return m_WindowState; }

	private:
		smuint64 m_Width = 0;
		smuint64 m_Height = 0;
		smuint64 m_X = 0;
		smuint64 m_Y = 0;
		smcstring m_WindowName = nullptr;
		WindowState* m_WindowState = nullptr;
	};
}

END_NAMESPACE