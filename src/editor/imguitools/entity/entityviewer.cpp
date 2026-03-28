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

    World* world = GET_WORLD();
    ECS::World& ecsWorld = world->GetESCWorld();

    PersistentId persistentId = ecsWorld.Get<PersistentId>(pickedEntity);

    ImGui::Text("Entity %u", persistentId.m_Id);

    std::string nameBuffer {};
    ImGui::InputText("Entity Name", &nameBuffer);

    if (ImGui::Button("Add Entity"))
    {
        ECS::EntityId entity = ecsWorld.Spawn();
        world->SaveAll();
    }

    ImGui::Separator();

    const std::vector<ECS::EntityId>& entities = ecsWorld.GetEntityIds();
    for (ECS::EntityId id : entities)
    {
        ImGui::Text("Entity %u", id);
    }
}

END_NAMESPACE
#endif
#endif
