#include <engine/world/world.hpp>

// Static member definition required by the linker
std::vector<smstring> samaritan::ECS::World::ms_ComponentDebugLabels;

#include <engine/components/persistentid.hpp>
#include <engine/components/transform.hpp>
#include <engine/components/visualcomponent.hpp>

#include <format>
#include <fstream>
#include <nlohmann/json.hpp>

BEGIN_NAMESPACE

void World::OnLoad()
{
    const std::string path = std::format("assets/worlds/{}", m_Name);

    std::ifstream f(path);
    if (f.fail())
    {
        LogError(LogChannel::Graphics, "Cannot open world file '%s'!", path.c_str());
        m_State = ResourceState::Error;
        return;
    }

    nlohmann::json data = nlohmann::json::parse(f);

    m_Name = data["name"].template get<std::string>();
    m_DBName = data["file"].template get<std::string>();
    m_StartingPosition = smVec3::StringToVec3(data["startingPosition"]);

    const std::string dbPath = std::format("assets/worlds/{}", m_DBName);
    if (sqlite3_open(dbPath.c_str(), &m_Db) != SQLITE_OK)
    {
        LogError(LogChannel::Graphics, "Cannot open database '%s'!", dbPath.c_str());
        m_State = ResourceState::Error;
        return;
    }

    m_ECSWorld.RegisterComponent<PersistentId>();
    m_ECSWorld.RegisterComponent<Transform>();
    m_ECSWorld.RegisterComponent<VisualComponent>();

    m_WorldSerializer.Init(m_Db);
    m_WorldSerializer.Register<Transform>();
    m_WorldSerializer.Register<VisualComponent>();
    m_WorldSerializer.LoadAll(m_ECSWorld);

    m_Type  = ResourceType::World;
    m_State = ResourceState::Loaded;
}

void World::SaveAll()
{
    m_WorldSerializer.SaveAll(m_ECSWorld);
}

void World::OnUnload()
{
    if (m_Db)
    {
        sqlite3_close(m_Db);
        m_Db = nullptr;
    }
}

END_NAMESPACE