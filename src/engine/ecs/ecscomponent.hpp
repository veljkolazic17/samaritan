#pragma once
#include <engine/ecs/ecstypes.hpp>

BEGIN_NAMESPACE

namespace ECS
{
    struct Component
    {
    };

    struct Tag final : public Component 
    {
    };
}

END_NAMESPACE