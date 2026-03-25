#pragma once
#include <defines.hpp>
#include <engine/ecs/ecscomponent.hpp>

BEGIN_NAMESPACE

struct PersistentId : public ECS::Component
{
    PersistentId() = default;
    explicit PersistentId(smuint64 id) : m_Id(id) {}

    smuint64 m_Id = 0;
};

END_NAMESPACE

#include <engine/world/serialization/componentserializer.hpp>
SERIALIZABLE_COMPONENT(
    samaritan::PersistentId,
    field(m_Id)
)
