#include <engine/Engine.hpp>
#define PROJECT_NAME "samaritanengine"



#ifdef TARGET_WIN
#ifdef WINDOW_NATIVE_WIN
#include <windows.h>
#include <windowsx.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    samaritan::Engine::Engine& engine = samaritan::Engine::Engine::GetInstance();

    engine.Init();
    engine.Run();
    engine.Shutdown();

    return 0;
}
#else
int main(void)
{
    samaritan::Engine::Engine& engine = samaritan::Engine::Engine::GetInstance();

    engine.Init();
    engine.Run();
    engine.Shutdown();

    return 0;
}
#endif
#endif
