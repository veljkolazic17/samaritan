#pragma once
#include <defines.hpp>

#include <engine/ecs/ecscomponent.hpp>

#include <math/vector.hpp>
#include <math/matrix.hpp>

BEGIN_NAMESPACE

struct Transform : public ECS::Component
{
    smVec3 m_Position;
    smVec3 m_Rotation;
    smVec3 m_Scale;
};

END_NAMESPACE

#include <engine/world/serialization/componentserializer.hpp>
SERIALIZABLE_COMPONENT(
    samaritan::Transform,
    field(m_Position),
    field(m_Rotation),
    field(m_Scale)
)
