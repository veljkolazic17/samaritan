#pragma once
#include <imguitools/entity/entityviewer.hpp>

#ifdef SM_TOOL
#if IMGUI_DISPLAY_ENABLED

#include <engine/world/worldmanager.hpp>
#include <engine/world/world.hpp>

BEGIN_NAMESPACE

void EntityViewer::DrawImgui()
{
    std::string nameBuffer {};
    ImGui::InputText("Entity Name", &nameBuffer);

    World* world = GET_WORLD();
    ECS::World& ecsWorld = world->GetESCWorld();

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
