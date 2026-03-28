#pragma once
#include <defines.hpp>
#include <engine/ecs/ecscomponent.hpp>

BEGIN_NAMESPACE

struct ParentComponent : public ECS::Component
{
    ParentComponent() = default;
    explicit ParentComponent(smuint64 id) : m_Id(id) {}

    smuint64 m_Id = 0;
};

END_NAMESPACE

#include <engine/world/serialization/componentserializer.hpp>
SERIALIZABLE_COMPONENT(
    samaritan::ParentComponent,
    field(m_Id)
)
