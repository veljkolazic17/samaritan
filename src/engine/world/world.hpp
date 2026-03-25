#pragma once
#include <defines.hpp>

#include <engine/world/serialization/worldserializer.hpp>
#include <engine/ecs/ecsworld.hpp>

#include <engine/resources/resource.hpp>
#include <sqlite3.h>

BEGIN_NAMESPACE

class World : public Resource
{
public:
    World() = default;
    World(const std::string& name) : Resource(name), m_Db(nullptr) {}

    void OnUnload() override;
    void OnLoad() override;

    SM_INLINE ECS::World& GetESCWorld() { return m_ECSWorld; }
    SM_INLINE const ECS::World& GetESCWorld() const { return m_ECSWorld; }
    SM_INLINE sqlite3* GetDb() const { return m_Db; }

    void SaveAll();

private:
    WorldSerializer m_WorldSerializer;
    ECS::World m_ECSWorld;

    sqlite3* m_Db;
    smVec3 m_StartingPosition;
    smstring m_DBName;
};

END_NAMESPACE