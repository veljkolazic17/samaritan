#ifdef SM_TOOL
#include <engine/graphics/debug/transformdebug.hpp>

#if IMGUI_DISPLAY_ENABLED
#include "imgui.h"
#endif

BEGIN_NAMESPACE

void TransformDebug::SingletonInit()
{
    IMGUI_DISPLAY(smImguiCentral().RegisterImguiModule("Debug/Transform", this);)
}

#if IMGUI_DISPLAY_ENABLED
void TransformDebug::DrawImgui()
{
    ImGui::SliderFloat("Rotation X", &m_Rotation[0], -180.0f, 180.0f, "%.1f deg");
    ImGui::SliderFloat("Rotation Y", &m_Rotation[1], -180.0f, 180.0f, "%.1f deg");
    ImGui::SliderFloat("Rotation Z", &m_Rotation[2], -180.0f, 180.0f, "%.1f deg");
    if (ImGui::Button("Reset"))
    {
        m_Rotation[0] = 0.0f;
        m_Rotation[1] = 0.0f;
        m_Rotation[2] = 0.0f;
    }
}
#endif

END_NAMESPACE
#endif // SM_TOOL
