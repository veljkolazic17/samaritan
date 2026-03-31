#pragma once
#include <defines.hpp>

#ifdef SM_TOOL
#if IMGUI_DISPLAY_ENABLED
#include <editor/imgui/imguicentral.hpp>

#include <refl.hpp>

BEGIN_NAMESPACE

namespace ECS
{
    class World;
}

class EntityViewer : public IImguiModule
{
public:
    void DrawImgui() override;
    smstring GetPath() { return "Engine/Entities/Entity Viewer"; }

private:
    void DrawComponents(size_t entityId, ECS::World& ecsWorld);
    size_t m_PickedEntity = UINT_MAX;
};

END_NAMESPACE
#endif
#endif
