#pragma once
#include <defines.hpp>
#include <engine/ecs/ecsworld.hpp>
#include <objecttemplates/singleton.hpp>
#include <engine/ecs/systemregistry.hpp>
#include <functional>
#include <vector>

#define smECSSystems() ::samaritan::ECS::SystemManager::GetInstance()

BEGIN_NAMESPACE

namespace ECS
{

class SystemManager SINGLETON(SystemManager)
{
public:
    SINGLETON_CONSTRUCTOR(SystemManager)

    void SingletonInit() override { RegisterSystems(); }

    using System = std::function<void(World&)>;

    void Register(System system)
    {
        m_Systems.push_back(std::move(system));
    }

    void UpdateAll(World& world)
    {
        for (auto& system : m_Systems)
        {
            system(world);
        }
    }

private:
    std::vector<System> m_Systems;
};

}

END_NAMESPACE
