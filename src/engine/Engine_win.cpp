#ifdef TARGET_WIN
#include <engine/Engine.hpp>
#include <windows.h>

BEGIN_NAMESPACE

namespace Engine
{
	void Engine::LoopPreProcessPlatformImpl(void)
	{
		MSG message;
		while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}
	}
}

END_NAMESPACE
#endif