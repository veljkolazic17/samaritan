#include <engine/world/worldmanager.hpp>

#include <engine/resources/resourcesystem.hpp>

BEGIN_NAMESPACE

void WorldManager::SingletonInit()
{
    const smstring defaultWorldPath = "defaultWorld.json";
    m_World = smResourceSystem().Load<World>(defaultWorldPath);
}

void WorldManager::SingletonShutdown()
{

}

END_NAMESPACE