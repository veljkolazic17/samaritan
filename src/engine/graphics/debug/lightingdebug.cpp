#ifdef SM_TOOL
#include <engine/graphics/debug/lightingdebug.hpp>

#if IMGUI_DISPLAY_ENABLED
#include "imgui.h"
#include <engine/graphics/systems/materialsystem.hpp>
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

    ImGui::Separator();
    ImGui::Text("Material Shininess");

    MaterialSystem& matSys = smMaterialSystem();

    ImGui::PushID("default");
    ImGui::SliderFloat(SM_DEFAULT_MATERIAL_NAME, &matSys.GetDefaultMaterial().m_Shininess, 1.0f, 256.0f);
    ImGui::PopID();

    for (const auto& [name, ref] : matSys.GetMaterialLookup())
    {
        if (ref.m_Handle == SM_INVALID_ID)
        {
            continue;
        }

        Material& material = matSys.GetMaterialByHandle(ref.m_Handle);
        ImGui::PushID(name.c_str());
        ImGui::SliderFloat(name.c_str(), &material.m_Shininess, 1.0f, 256.0f);
        ImGui::PopID();
    }
}
#endif

END_NAMESPACE
#endif // SM_TOOL
