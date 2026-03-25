#include <engine/graphics/systems/visualsystem.hpp>

#include <engine/components/visualcomponent.hpp>
#include <engine/components/transform.hpp>
#include <engine/graphics/systems/meshsystem.hpp>
#include <engine/graphics/renderer/frontend/rendererfrontend.hpp>
#include <engine/ecs/ecsquery.hpp>

BEGIN_NAMESPACE

void ECS::VisualSystem(ECS::World& world)
{
    ECS::Query<VisualComponent, Transform> query(&world);
    query.Foreach([](ECS::EntityId entity, VisualComponent& vc, Transform& t)
    {
        if (!vc.m_MeshPath.empty() && !vc.m_Mesh)
        {
            vc.m_Mesh = smMeshSystem().Acquire(vc.m_MeshPath);
        }

        if (!vc.m_Mesh)
        {
            return;
        }
        
        smMat4 translation = smMat4Translation(t.m_Position);
        smMat4 rotation = smMat4::RotationEuler(t.m_Rotation.m_X, t.m_Rotation.m_Y, t.m_Rotation.m_Z);
        smMat4 scale = smMat4
        (
            smVec4(t.m_Scale.m_X, 0, 0, 0),
            smVec4(0, t.m_Scale.m_Y, 0, 0),
            smVec4(0, 0, t.m_Scale.m_Z, 0),
            smVec4(0, 0, 0, 1)
        );

        smMat4 model = translation * rotation * scale;

        smRenderer().SubmitMesh(vc.m_Mesh.GetResource(), model);
    });
}

END_NAMESPACE
