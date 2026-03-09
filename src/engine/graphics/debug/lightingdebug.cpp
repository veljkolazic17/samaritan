#ifdef SM_TOOL
#include <engine/graphics/debug/lightingdebug.hpp>

#if IMGUI_DISPLAY_ENABLED
#include "imgui.h"
#endif

BEGIN_NAMESPACE

void LightingDebug::SingletonInit()
{
    IMGUI_DISPLAY(smImguiCentral().RegisterImguiModule("Debug/Lighting", this);)
}

#if IMGUI_DISPLAY_ENABLED
void LightingDebug::DrawImgui()
{
    ImGui::ColorEdit3("Ambient Color", &m_AmbientColor.m_X);
}
#endif

END_NAMESPACE
#endif // SM_TOOL
