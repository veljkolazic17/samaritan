#pragma once
#include <defines.hpp>
#if IMGUI_DISPLAY_ENABLED

#include <objecttemplates/singleton.hpp>

#define smImguiDrawModule() ::samaritan::ImguiDrawModule::GetInstance()

BEGIN_NAMESPACE

class ImguiDrawModule SINGLETON(ImguiDrawModule)
{
public:
    SINGLETON_CONSTRUCTOR(ImguiDrawModule);

    void Init();
    void NewFrame();
    void Render();
    void Shutdown();
    void DrawImgui();

private:
    void InitRenderer_PlatformSpecific();
    void InitWindow_PlatformSpecific();
    void InitInputs_PlatformSpecific();

    void NewFrame_PlatformSpecific();
    void Render_PlatformSpecific();
    void Shutdown_PlatformSpecific();
};

END_NAMESPACE
#endif
