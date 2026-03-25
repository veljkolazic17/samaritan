#pragma once
#include <defines.hpp>

#include <engine/world/world.hpp>

#include <engine/resources/resource.hpp>
#include <objecttemplates/singleton.hpp>

#define worldManager() ::samaritan::WorldManager::GetInstance()
#define GET_WORLD() ::samaritan::WorldManager::GetInstance().GetWorld()

BEGIN_NAMESPACE

class WorldManager SINGLETON(WorldManager)
{
public:
    SINGLETON_CONSTRUCTOR(WorldManager)

    void SingletonInit() override;
    void SingletonShutdown() override;

    SM_INLINE const World* GetWorld() const { return m_World.GetResource(); }
    SM_INLINE World* GetWorld() { return m_World.GetResource(); }

private:
    ResourceHandle<World> m_World;
};

END_NAMESPACE