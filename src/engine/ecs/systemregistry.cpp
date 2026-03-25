#include <defines.hpp>
#include <engine/ecs/ecssystemmanager.hpp>
#include <engine/graphics/systems/visualsystem.hpp>

#define REGISTER_SYSTEM(system) smECSSystems().Register(system)

BEGIN_NAMESPACE

namespace ECS
{
void RegisterSystems()
{
    REGISTER_SYSTEM(ECS::VisualSystem);
}
}

END_NAMESPACE
