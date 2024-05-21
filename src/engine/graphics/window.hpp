#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

// Forward declarations
namespace Graphics
{
	struct WindowState;
}

namespace Graphics
{
	class Window
	{
	public:
		Window(muint64 width, muint64 height, muint64 x, muint64 y, mcstring windowName)
			: m_Width(width),
			m_Height(height),
			m_X(x),
			m_Y(y),
			m_WindowName(windowName){}
		Window(){}

		inline muint64 GetWidth() { return m_Width; }
		inline muint64 GetHeight() { return m_Height; }

		inline void SetWidth(muint64 width) { m_Width = width; }
		inline void SetHeight(muint64 height) { m_Height = height; }

		void Init();
		void Shutdown();

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