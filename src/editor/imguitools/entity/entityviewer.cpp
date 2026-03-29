#pragma once
#include <editor/imguitools/entity/entityviewer.hpp>

#ifdef SM_TOOL
#if IMGUI_DISPLAY_ENABLED

#include <editor/tools/entitypicker.hpp>

#include <engine/world/worldmanager.hpp>
#include <engine/world/world.hpp>

BEGIN_NAMESPACE

void EntityViewer::DrawImgui()
{
    smcstring text = m_EnableEntityPicking ? "Enable" : "Disable";
    if (ImGui::Button(text))
    {
        m_EnableEntityPicking != m_EnableEntityPicking;
    }
    smuint32 pickedEntity = smEntityPicker().GetPickedObjectId();
    if (pickedEntity == std::numeric_limits<unsigned int>::max()) //must use this because I have put 32bit for entity id in texture
    {
        return;
    }

    World* world = GET_WORLD();
    ECS::World& ecsWorld = world->GetESCWorld();

    PersistentId persistentId = ecsWorld.Get<PersistentId>(pickedEntity);

    ImGui::Text("ECS Entity Id %u", pickedEntity);
    ImGui::Text("Persistent Entity Id %u", persistentId.m_Id);
}

END_NAMESPACE
#endif
#endif
