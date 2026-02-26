#include <defines.hpp>
#if IMGUI_DISPLAY_ENABLED
#include <imgui/imguidrawmodule.hpp>

#include "imgui.h"

BEGIN_NAMESPACE

void ImguiDrawModule::Init()
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

void ImguiDrawModule::Shutdown()
{
    Shutdown_PlatformSpecific();
}

void ImguiDrawModule::DrawImgui()
{
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("test")) {
        if (ImGui::MenuItem("one", "Ctrl+O")) {}
        if (ImGui::MenuItem("two", "Ctrl+S")) {}
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

END_NAMESPACE
#endif
