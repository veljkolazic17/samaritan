#pragma once
#include <defines.hpp>

#ifdef SM_TOOL
#if IMGUI_DISPLAY_ENABLED

#include <imgui/imguicentral.hpp>

BEGIN_NAMESPACE

class WorldViewer : public IImguiModule
{
public:
    void DrawImgui() override;
    smstring GetPath() { return "Engine/World/World Viewer"; }
};

END_NAMESPACE
#endif
#endif