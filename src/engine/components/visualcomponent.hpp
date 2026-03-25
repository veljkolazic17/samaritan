#pragma once
#include <defines.hpp>

#include <engine/ecs/ecscomponent.hpp>

#include <engine/resources/resource.hpp>
#include <engine/resources/graphics/mesh.hpp>

BEGIN_NAMESPACE

//Don't like this I have mesh path saved 2 times
struct VisualComponent : public ECS::Component
{
    smstring m_MeshPath;
    ResourceHandle<Mesh> m_Mesh;
};

END_NAMESPACE

#include <engine/world/serialization/componentserializer.hpp>
SERIALIZABLE_COMPONENT(samaritan::VisualComponent, field(m_MeshPath))
