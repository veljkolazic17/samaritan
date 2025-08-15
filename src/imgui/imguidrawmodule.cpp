#include <defines.hpp>
#if IMGUI_DISPLAY_ENABLED
#include <imgui/imguidrawmodule.hpp>

BEGIN_NAMESPACE

void ImguiDrawModule::Init()
{
    InitWindow_PlatformSpecific();
    InitRenderer_PlatformSpecific();
    InitInputs_PlatformSpecific();
}

END_NAMESPACE
#endif