#pragma once
#include <defines.hpp>

#ifdef SM_TOOL
#if IMGUI_DISPLAY_ENABLED

#include <editor/imgui/imguicentral.hpp>

BEGIN_NAMESPACE

class EntityViewer : public IImguiModule
{
public:
    void DrawImgui() override;
    smstring GetPath() { return "Engine/Entities/Entity Viewer"; }

private:
    smbool m_EnableEntityPicking = false;
};

END_NAMESPACE
#endif
#endif
