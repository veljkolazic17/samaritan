#pragma once
#include <editor/imguitools/entity/entityviewer.hpp>

#ifdef SM_TOOL
#if IMGUI_DISPLAY_ENABLED

#include <editor/tools/entitypicker.hpp>
#include <editor/imguitools/imguitypehelper.hpp>

#include <engine/world/worldmanager.hpp>
#include <engine/world/world.hpp>

BEGIN_NAMESPACE

void EntityViewer::DrawImgui()
{
    smuint32 pickedEntity = smEntityPicker().GetPickedObjectId();
    if (pickedEntity != std::numeric_limits<unsigned int>::max()) //must use this because I have put 32bit for entity id in texture
    {
        m_PickedEntity = pickedEntity;
    }

    //need to ckek if not max for first time when no entitu is picked
    if (m_PickedEntity != std::numeric_limits<unsigned int>::max())
    {
        World* world = GET_WORLD();
        ECS::World& ecsWorld = world->GetESCWorld();

        PersistentId persistentId = ecsWorld.Get<PersistentId>(m_PickedEntity);

        ImGui::Text("ECS Entity Id %u", m_PickedEntity);
        ImGui::Text("Persistent Entity Id %u", persistentId.m_Id);

        DrawComponents(m_PickedEntity, ecsWorld);
    }
}

void EntityViewer::DrawComponents(size_t entityId, ECS::World& ecsWorld)
{
    ecsWorld.ForEachComponent(entityId, [&](const smstring& componentName, ECS::ISparseSet& storage)
    {
        if (ImGui::CollapsingHeader(componentName.c_str()))
        {
            storage.VisitMembers(entityId, [](const char* memberName, const char* typeName, void* memberPtr, size_t memberSize)
            {
                TypeHelper::DrawType(memberName, typeName, memberPtr);
            });
        }
    });
}

END_NAMESPACE
#endif
#endif
