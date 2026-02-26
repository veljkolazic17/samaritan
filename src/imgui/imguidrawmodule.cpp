#include <defines.hpp>
#if IMGUI_DISPLAY_ENABLED
#include <imgui/imguidrawmodule.hpp>
#include <imgui/imguicentral.hpp>

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
    if (ImGui::BeginMainMenuBar())
    {
        smImguiCentral().DrawMenuBar();
        ImGui::EndMainMenuBar();
    }

    smImguiCentral().DrawOpenModules();
}

END_NAMESPACE
#endif
