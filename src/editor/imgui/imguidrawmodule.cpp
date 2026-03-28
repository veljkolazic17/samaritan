#include <defines.hpp>
#if IMGUI_DISPLAY_ENABLED
#include <editor/imgui/imguidrawmodule.hpp>
#include <editor/imgui/imguicentral.hpp>

#include "imgui.h"

BEGIN_NAMESPACE

void ImguiDrawModule::SingletonInit()
{
    InitWindow_PlatformSpecific();
    InitRenderer_PlatformSpecific();
    InitInputs_PlatformSpecific();
}

void ImguiDrawModule::NewFrame()
{
    NewFrame_PlatformSpecific();
}

void ImguiDrawModule::Render()
{
    Render_PlatformSpecific();
}

void ImguiDrawModule::SingletonShutdown()
{
    Shutdown_PlatformSpecific();
}

void ImguiDrawModule::DrawImgui()
{
    smImguiCentral().DrawImgui();
}

END_NAMESPACE
#endif
